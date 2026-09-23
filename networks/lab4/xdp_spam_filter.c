// Версия 2: блокируем TCP-пакеты, где в данных есть слово "SPAM"
// Это вторая часть задания — Deep Packet Inspection

#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <bpf/bpf_helpers.h>

SEC("xdp")
int xdp_spam_hunter(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data     = (void *)(long)ctx->data;

    // L2: Ethernet
    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end) return XDP_PASS;
    if (eth->h_proto != __constant_htons(ETH_P_IP)) return XDP_PASS;

    // L3: IP
    struct iphdr *iph = data + sizeof(struct ethhdr);
    if ((void *)(iph + 1) > data_end) return XDP_PASS;
    if (iph->protocol != IPPROTO_TCP) return XDP_PASS;

    // L4: TCP
    // iph->ihl * 4 — длина IP-заголовка в байтах
    struct tcphdr *tcph = (void *)iph + (iph->ihl * 4);
    if ((void *)(tcph + 1) > data_end) return XDP_PASS;

    // Сужаем работу до соединений на порту 8080 (можно убрать — будет искать везде)
    if (tcph->dest != __constant_htons(8080) && tcph->source != __constant_htons(8080)) {
        return XDP_PASS;
    }

    // === Поиск SPAM в payload ===
    // tcph->doff — Data Offset, длина TCP-заголовка в словах по 4 байта
    // payload начинается сразу после TCP-заголовка
    unsigned char *payload = (unsigned char *)tcph + (tcph->doff * 4);

    // #pragma unroll — подсказка компилятору развернуть цикл
    // Verifier требует, чтобы число итераций было известно на этапе компиляции
    #pragma unroll
    for (int i = 0; i < 128; i++) {
        // КРИТИЧЕСКАЯ проверка: вычисляем адрес payload[i+3] и убеждаемся, что он в пределах пакета
        // Без этого Verifier откажется грузить — он защищает от чтения чужой памяти
        if ((void *)(payload + i + 4) > data_end) {
            break;
        }
        // Посимвольное сравнение S-P-A-M
        if (payload[i] == 'S' && payload[i + 1] == 'P' &&
            payload[i + 2] == 'A' && payload[i + 3] == 'M') {
            bpf_printk("!!! SPAM FOUND at offset %d !!!\n", i);
            return XDP_DROP;
        }
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
