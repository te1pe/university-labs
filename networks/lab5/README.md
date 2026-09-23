# Лабораторная 5 — WWW, DNS, HA Cluster

**Курс:** Компьютерные сети, 01.09.2025–31.12.2025

Построение отказоустойчивой высоконагруженной инфраструктуры для Flask-приложения:
кластер из трёх нод Debian 12 с балансировкой, синхронной репликацией БД,
распределённым объектным хранилищем и собственным центром сертификации.

## Стенд

| Нода | IP | Роль |
| --- | --- | --- |
| node1 | 192.168.64.11 | все сервисы + step-ca |
| node2 | 192.168.64.12 | все сервисы |
| node3 | 192.168.64.13 | все сервисы |
| VIP | 192.168.64.100 | плавающий адрес кластера (Pacemaker) |

Виртуализация — **UTM/QEMU** на Apple Silicon, Debian 12 aarch64. Методичка написана
под VirtualBox + Vagrant, поэтому часть шагов заменена: VM создаются вручную вместо
`vagrant up`, а STONITH через `fence_vbox` не используется (см. «Отступления»).

## Архитектура

```
                    клиент
                      │
                 VIP 192.168.64.100         ← Pacemaker переносит между нодами
                      │
                  HAProxy :443              ← TLS от локального step-ca
                      │
        ┌─────────────┼─────────────┐
     Flask:5000   Flask:5000    Flask:5000  ← gunicorn, 4 воркера
        │             │             │
    ProxySQL:6033 ProxySQL:6033 ProxySQL:6033 ← роутинг SELECT/INSERT
        │             │             │
    ┌───┴─────────────┴─────────────┴───┐
    │      MariaDB Galera (multi-master) │   ← синхронная репликация
    └────────────────────────────────────┘

    MinIO Distributed :9000   ← объектное хранилище, 2 диска на ноду
    dnsdist :5353             ← кэширующий DNS с healthcheck и лимитом QPS
    Corosync/Pacemaker        ← кворум и управление ресурсами
```

Ключевая идея — вынести состояние за пределы приложения. Flask stateless и работает
на всех трёх нодах одновременно; данные живут в Galera, файлы в MinIO, маршрутизация
запросов в ProxySQL, сетевое состояние кластера в Corosync.

## Структура

```
lab5/
├── configs/
│   ├── corosync.conf              кластер: 3 ноды, knet, шифрование aes256
│   ├── haproxy.cfg                L7-балансировка Flask, TCP для MySQL, MinIO
│   ├── mariadb-60-galera.cnf      Galera multi-master
│   ├── mariadb-61-bind.cnf        bind-address на адрес ноды
│   ├── proxysql_setup.sql         hostgroups, пользователи, galera-мониторинг
│   ├── dnsdist.conf               резолверы, healthcheck, ACL, лимит QPS
│   ├── minio.env                  /etc/default/minio
│   ├── hosts                      маппинг имён нод
│   └── interfaces.example         статическая адресация
├── systemd/
│   ├── flaskapp.service           gunicorn, 4 воркера
│   ├── minio.service              MinIO Distributed
│   ├── step-ca.service            локальный CA
│   └── tc-shaping.service         запуск шейпера при загрузке
├── scripts/
│   ├── pacemaker-setup.sh         VIP + HAProxy как ресурсы кластера
│   └── tc-setup.sh                HTB-шейпинг по приоритетам трафика
├── app/
│   ├── app.py                     Flask: /health, /db/items
│   └── requirements.txt
└── highload-ha-lab-v5.1.docx      методичка
```

## Развёртывание

```bash
# 0. сеть и имена (на каждой ноде)
sudo cp configs/hosts /etc/hosts
sudo cp configs/interfaces.example /etc/network/interfaces   # поправить адрес

# 1. локальный CA (только node1)
sudo cp systemd/step-ca.service /etc/systemd/system/
sudo systemctl enable --now step-ca

# 2. кластер (на каждой ноде)
sudo cp configs/corosync.conf /etc/corosync/corosync.conf
sudo systemctl restart corosync pacemaker

# 3. балансировщик
sudo cp configs/haproxy.cfg /etc/haproxy/haproxy.cfg

# 4. БД
sudo cp configs/mariadb-60-galera.cnf /etc/mysql/mariadb.conf.d/60-galera.cnf
sudo galera_new_cluster                    # только на первой ноде
sudo systemctl start mariadb               # на остальных

# 5. ProxySQL
mysql -uadmin -padmin -h127.0.0.1 -P6032 < configs/proxysql_setup.sql

# 6. DNS
sudo cp configs/dnsdist.conf /etc/dnsdist/dnsdist.conf

# 7. приложение
sudo cp app/app.py /opt/flaskapp/ && sudo cp systemd/flaskapp.service /etc/systemd/system/
sudo systemctl enable --now flaskapp

# 8. хранилище
sudo cp configs/minio.env /etc/default/minio && sudo chmod 600 /etc/default/minio
sudo cp systemd/minio.service /etc/systemd/system/
sudo systemctl enable --now minio

# 9. ресурсы кластера — один раз с любой ноды
./scripts/pacemaker-setup.sh
```

## Проверка отказоустойчивости

```bash
sudo pcs status                       # где сейчас VIP и HAProxy
mysql -e "SHOW STATUS LIKE 'wsrep_cluster_size'"   # должно быть 3
curl -k https://192.168.64.100/health              # ответ от любой ноды

# гасим ноду с VIP — ресурсы должны переехать
sudo pcs node standby node1
sudo pcs status
```

## Что показывает работа

Кворум: при трёх нодах кластер переживает отказ одной (2 из 3 голосов). При двух
упавших Pacemaker останавливает ресурсы вместо того, чтобы поднять их на последней
ноде — иначе при восстановлении связи получилось бы два кластера с одним VIP
(split-brain).

Galera синхронна: транзакция подтверждается только после того, как её приняли
остальные ноды. Цена — задержка записи выше, чем у одиночной БД; выигрыш — с любой
ноды видны одни и те же данные сразу, без лага репликации.

ProxySQL знает про Galera через `mysql_galera_hostgroups`: writer-группа (10) держит
одну ноду, reader-группа (20) — все три, а при отказе writer'а роль автоматически
переходит к ноде из backup-группы (30). Без этого приложению пришлось бы самому
решать, куда слать `INSERT`.
