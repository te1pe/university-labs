#!/bin/bash
# Настройка ресурсов Pacemaker. Выполняется один раз с любой ноды кластера.
set -e

# --- свойства кластера ---
# STONITH отключён: fence_vbox требует VirtualBox на хосте, а кластер живёт в UTM/QEMU.
# По критериям лабораторной это стоит балла — см. README.
sudo pcs property set stonith-enabled=false
sudo pcs property set no-quorum-policy=stop
sudo pcs resource defaults update resource-stickiness=100
sudo pcs resource defaults update migration-threshold=3

# --- виртуальный IP кластера ---
sudo pcs resource create vip_cluster ocf:heartbeat:IPaddr2 \
    ip=192.168.64.100 \
    cidr_netmask=24 \
    nic=eth0 \
    op monitor interval=10s timeout=20s

# --- HAProxy как ресурс кластера ---
sudo pcs resource create haproxy_svc systemd:haproxy \
    op monitor interval=20s timeout=30s \
    op start timeout=60s \
    op stop timeout=60s

# --- HAProxy всегда там же, где VIP, и стартует после него ---
sudo pcs constraint colocation add haproxy_svc with vip_cluster INFINITY
sudo pcs constraint order start vip_cluster then start haproxy_svc

sudo pcs constraint
sudo pcs status --no-pager
