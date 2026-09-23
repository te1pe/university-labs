#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <bpf/bpf_helpers.h>

#define BLOCKED_IP   0x7F000001
#define BLOCKED_PORT 8080

SEC("xdp")
int xdp_filter_basic(struct xdp_md *ctx) {
    void *data_end = (void *)ctx->data_end;
    void *data     = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end) return XDP_PASS;
    if (eth->h_proto != __constant_htons(ETH_P_IP)) return XDP_PASS;

    struct iphdr *iph = data + sizeof(struct ethhdr);
    if ((void *)(iph + 1) > data_end) return XDP_PASS;
    if (iph->protocol != IPPROTO_TCP) return XDP_PASS;

    struct tcphdr *tcph = (void *)iph + (iph->ihl * 4);
    if ((void *)(tcph + 1) > data_end) return XDP_PASS;

    if (iph->saddr == __constant_htonl(BLOCKED_IP) &&
        tcph->dest == __constant_htons(BLOCKED_PORT)) {
        bpf_printk("BLOCK: src=127.0.0.1 dst_port=8080\n");
        return XDP_DROP;
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
