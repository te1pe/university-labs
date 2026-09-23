// Версия 1: блокируем входящие TCP-пакеты на порт 8080
// Это первая часть задания — фильтрация по порту

#include <linux/bpf.h>           // основные определения eBPF (XDP_PASS, XDP_DROP)
#include <linux/if_ether.h>      // структура ethhdr — Ethernet-заголовок (L2)
#include <linux/ip.h>            // структура iphdr — IP-заголовок (L3)
#include <linux/tcp.h>           // структура tcphdr — TCP-заголовок (L4)
#include <linux/in.h>            // константы протоколов (IPPROTO_TCP, ETH_P_IP)
#include <bpf/bpf_helpers.h>     // макрос SEC(), функция bpf_printk

// SEC("xdp") — кладём функцию в секцию ELF-файла с именем "xdp"
// При загрузке через `ip link ... sec xdp` ядро найдёт именно эту секцию
SEC("xdp")
int xdp_filter_basic(struct xdp_md *ctx) {
    // ctx->data и ctx->data_end — границы пакета в памяти
    // Приведение через (long) гарантирует, что значение влезет в 64-битный указатель
    void *data_end = (void *)(long)ctx->data_end;
    void *data     = (void *)(long)ctx->data;

    // === L2: Ethernet ===
    // "Накладываем трафарет" Ethernet-заголовка на начало пакета
    struct ethhdr *eth = data;
    // ОБЯЗАТЕЛЬНАЯ проверка границ: eth + 1 = адрес сразу после заголовка
    // Если он за пределами пакета — пакет битый, пропускаем
    // Без этой проверки Verifier откажется грузить программу
    if ((void *)(eth + 1) > data_end) return XDP_PASS;

    // ETH_P_IP = IPv4. Если это что-то другое (IPv6, ARP) — нам не интересно
    // __constant_htons переворачивает байты, т.к. сеть big-endian, а CPU little-endian
    if (eth->h_proto != __constant_htons(ETH_P_IP)) return XDP_PASS;

    // === L3: IP ===
    // Отступаем от начала пакета ровно на размер Ethernet-заголовка
    struct iphdr *iph = data + sizeof(struct ethhdr);
    if ((void *)(iph + 1) > data_end) return XDP_PASS;

    // Если это не TCP (а UDP/ICMP) — нам не интересно, пропускаем
    if (iph->protocol != IPPROTO_TCP) return XDP_PASS;

    // === L4: TCP ===
    // ВАЖНО: длина IP-заголовка варьируется (минимум 20 байт, может быть больше с опциями)
    // iph->ihl хранит длину в "словах" по 4 байта. Поэтому * 4
    struct tcphdr *tcph = (void *)iph + (iph->ihl * 4);
    if ((void *)(tcph + 1) > data_end) return XDP_PASS;

    // === Логика фильтра: блокируем порт 8080 в обоих направлениях ===
    // На loopback (lo) пакеты ходят в обе стороны через тот же интерфейс
    // Поэтому проверяем и dest, и source
    if (tcph->dest == __constant_htons(8080) || tcph->source == __constant_htons(8080)) {
        // bpf_printk — как printf, но пишет в /sys/kernel/debug/tracing/trace_pipe
        bpf_printk("BLOCK: TCP packet on port 8080\n");
        return XDP_DROP;  // уничтожить пакет, выше по стеку он не попадёт
    }

    return XDP_PASS;  // всё остальное — пропустить
}

// Лицензия. Без неё ядро не даст использовать большинство helper-функций
char _license[] SEC("license") = "GPL";
