# university-labs

Lab work from my time at NSU FIT, collected into a single repository. Each
assignment lives in its own folder together with a README: the task, how to
build and run it, and which decisions were made and why.

The per-lab READMEs are in Russian.

## Courses

| Course | Folder | Period | Language | Labs |
| --- | --- | --- | --- | --- |
| C programming | [`programming-c`](programming-c) | 2024/25 | C | 13 |
| OOP in C++ | [`oop-c++`](oop-c++) | autumn 2025 | C++ | 7 |
| OOP in Java | [`oop-java`](oop-java) | 2025/26 | Java, Spring Boot | 3 |
| Computer architecture | [`evm`](evm) | autumn 2025 | C, C++, x86/ARM asm | 10 |
| Operating systems | [`operation-system`](operation-system) | autumn 2025 | C, POSIX | 7 |
| Computer networks | [`networks`](networks) | autumn 2025 | C, eBPF, configuration | 5 |
| Parallel programming | [`parallel-programming`](parallel-programming) | spring 2026 | C, MPI, OpenMP, pthreads | 5 |
| Databases | [`databases`](databases) | 2026 | SQL | 1 |
| OOAD | [`ooad`](ooad) | from autumn 2026 | — | — |
| Translation methods | [`translation-methods`](translation-methods) | from autumn 2026 | C | — |

## The more interesting bits

- [`programming-c/lab5`](programming-c/lab5) — a Huffman archiver in plain C,
  with a 4 MB limit on the executable plus its data.
- [`networks/lab4`](networks/lab4) — packet filtering inside the Linux kernel
  with eBPF/XDP, by source IP and destination port.
- [`networks/lab5`](networks/lab5) — a fault-tolerant three-node cluster:
  HAProxy, Pacemaker, MariaDB Galera, ProxySQL, MinIO and a local step-ca.
- [`parallel-programming`](parallel-programming) — the same linear-system
  problem solved with MPI, OpenMP and pthreads, with timings and a comparison.
- [`evm/lab8`](evm/lab8), [`evm/lab9`](evm/lab9), [`evm/lab10`](evm/lab10) —
  determining L1/L2 cache size and associativity experimentally.
- [`oop-java/barbershop`](oop-java/barbershop) — a Spring Boot web app with
  sign-up, a service catalogue and booking.
- [`oop-java/chat`](oop-java/chat) — a multi-room chat over WebSocket with a
  JavaFX client, file transfer and message history.

## Building

There is no single build system: each lab has its own, described in its README.
Usually it is one command —

```bash
gcc -O2 -Wall -o lab lab.c      # C
g++ -std=c++17 -O2 -o lab *.cpp # C++
mvn spring-boot:run             # Java
```
