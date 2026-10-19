# university-labs

Лабораторные работы за время учёбы на ФИТ НГУ, собранные в один репозиторий.
Каждое задание лежит в своей папке вместе с README: условие, как собрать и
запустить, какие решения приняты и почему.

## Курсы

| Курс | Папка | Период | Язык | Лабораторных |
| --- | --- | --- | --- | --- |
| Программирование на C | [`programming-c`](programming-c) | 2024/25 | C | 13 |
| ООП на C++ | [`oop-c++`](oop-c++) | осень 2025 | C++ | 7 |
| ООП на Java | [`oop-java`](oop-java) | 2025/26 | Java, Spring Boot | 3 |
| Архитектура ЭВМ | [`evm`](evm) | осень 2025 | C, C++, ассемблер x86/ARM | 10 |
| Операционные системы | [`operation-system`](operation-system) | осень 2025 | C, POSIX | 7 |
| Компьютерные сети | [`networks`](networks) | осень 2025 | C, eBPF, конфигурация | 5 |
| Параллельное программирование | [`parallel-programming`](parallel-programming) | весна 2026 | C, MPI, OpenMP, pthreads | 5 |
| Базы данных | [`databases`](databases) | 2026 | SQL | 1 |
| ООАД | [`ooad`](ooad) | с осени 2026 | — | — |
| Методы трансляции | [`translation-methods`](translation-methods) | с осени 2026 | C | — |

## Самое интересное

- [`programming-c/lab5`](programming-c/lab5) — архиватор Хаффмана на чистом C
  с ограничением 4 МБ на исполняемый файл вместе с данными.
- [`networks/lab4`](networks/lab4) — фильтрация пакетов в ядре Linux через
  eBPF/XDP по IP-адресу источника и порту назначения.
- [`networks/lab5`](networks/lab5) — отказоустойчивый кластер из трёх узлов:
  HAProxy, Pacemaker, MariaDB Galera, ProxySQL, MinIO и локальный step-ca.
- [`parallel-programming`](parallel-programming) — одна и та же задача решения
  системы линейных уравнений на MPI, OpenMP и pthreads, с замерами времени и
  сравнением.
- [`evm/lab8`](evm/lab8), [`evm/lab9`](evm/lab9), [`evm/lab10`](evm/lab10) —
  экспериментальное определение размера и ассоциативности кэшей L1/L2.
- [`oop-java/barbershop`](oop-java/barbershop) — веб-приложение на Spring Boot
  с регистрацией, каталогом услуг и онлайн-записью.
- [`oop-java/chat`](oop-java/chat) — многокомнатный чат на WebSocket с клиентом
  на JavaFX, передачей файлов и историей сообщений.

## Сборка

Единой системы сборки нет: у каждой лабораторной своя, она описана в её README.
Обычно это одна команда:

```bash
gcc -O2 -Wall -o lab lab.c      # C
g++ -std=c++17 -O2 -o lab *.cpp # C++
mvn spring-boot:run             # Java
```
