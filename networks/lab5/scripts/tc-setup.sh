#!/bin/bash
set -e
tc qdisc del dev eth0 root 2>/dev/null || true
tc qdisc add dev eth0 root handle 1: htb default 30
tc class add dev eth0 parent 1: classid 1:1 htb rate 1000mbit ceil 1000mbit
tc class add dev eth0 parent 1:1 classid 1:10 htb rate 300mbit ceil 1000mbit prio 1
tc class add dev eth0 parent 1:1 classid 1:20 htb rate 400mbit ceil 1000mbit prio 2
tc class add dev eth0 parent 1:1 classid 1:30 htb rate 300mbit ceil 1000mbit prio 3
tc filter add dev eth0 protocol ip parent 1: prio 1 u32 match ip tos 0xc0 0xff flowid 1:10
tc filter add dev eth0 protocol ip parent 1: prio 2 u32 match ip tos 0x88 0xff flowid 1:20
tc qdisc add dev eth0 parent 1:10 handle 10: sfq perturb 10
tc qdisc add dev eth0 parent 1:20 handle 20: sfq perturb 10
tc qdisc add dev eth0 parent 1:30 handle 30: sfq perturb 10
