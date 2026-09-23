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

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end) return XDP_PASS;
    if (eth->h_proto != __constant_htons(ETH_P_IP)) return XDP_PASS;

    struct iphdr *iph = data + sizeof(struct ethhdr);
    if ((void *)(iph + 1) > data_end) return XDP_PASS;
    if (iph->protocol != IPPROTO_TCP) return XDP_PASS;

    struct tcphdr *tcph = (void *)iph + (iph->ihl * 4);
    if ((void *)(tcph + 1) > data_end) return XDP_PASS;

    unsigned char *payload = (unsigned char *)tcph + (tcph->doff * 4);

    #pragma unroll
    for (int i = 0; i < 128; i++) {
        if ((void *)(payload + i + 4) > data_end) {
            break;
        }
        if (payload[i]     == 'S' &&
            payload[i + 1] == 'P' &&
            payload[i + 2] == 'A' &&
            payload[i + 3] == 'M') {
            bpf_printk("!!! SPAM FOUND at offset %d !!!\n", i);
            return XDP_DROP;
        }
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
