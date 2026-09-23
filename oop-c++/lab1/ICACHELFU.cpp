#include "ICACHELFU.hpp"

LFU::LFU(size_t n) : cache_size(n) {}
void LFU::touch(NodesMapIt it) {
    auto node_it = it->second;
    int key  = node_it->key;
    int val  = node_it->data;
    int freq = node_it->freq;

    auto &old_list = freq_lists[freq];
    old_list.erase(node_it);
    if (old_list.empty()) {
        freq_lists.erase(freq);
        if (min_freq == freq) {
            ++min_freq;
        }
    }

    int new_freq = freq + 1;
    auto &new_list = freq_lists[new_freq];
    new_list.push_front({key, val, new_freq});
    it->second = new_list.begin();
}

int LFU::get(int key) {
    if (cache_size == 0) {
        return -1;
    }
    auto it = nodes.find(key);
    if (it == nodes.end()) {
        return -1;
    }
    touch(it);
    return it->second->data;
}

void LFU::put(int key, int data) {
    if (cache_size == 0) {
        return;
    }
    auto it = nodes.find(key);
    if (it != nodes.end()) {
        it->second->data = data;
        touch(it);
        return;
    }
    if (nodes.size() == cache_size) {
        auto &list_min = freq_lists[min_freq];
        auto &node_to_remove = list_min.back(); 
        int evict_key = node_to_remove.key;
        list_min.pop_back();
        if (list_min.empty()) {
            freq_lists.erase(min_freq);
        }
        nodes.erase(evict_key);
    }
    min_freq = 1;
    auto &list1 = freq_lists[1];
    list1.push_front({key, data, 1});
    nodes[key] = list1.begin();
}

int LFU::operator[](int key) {
    return get(key);
}
