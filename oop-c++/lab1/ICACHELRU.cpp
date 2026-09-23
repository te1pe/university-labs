#include "ICACHELRU.hpp"

LRU::LRU(size_t n): cache_size(n) {}

int LRU::get(int key){
    auto i=cache_map.find(key);
    if (i==cache_map.end()){
        return -1;
    };
    cache_list.splice(cache_list.begin(), cache_list, i->second);
    return i->second->second;
}

void LRU::put(int key, int data) {
    if (cache_size == 0) {
        return;
    }

    auto it = cache_map.find(key);
    if (it != cache_map.end()) {
        it->second->second = data;
        cache_list.splice(cache_list.begin(), cache_list, it->second);
        return;
    }

    if (cache_list.size() == cache_size) {
        int old_key = cache_list.back().first;
        cache_map.erase(old_key);
        cache_list.pop_back();
    }

    cache_list.push_front({key, data});
    cache_map[key] = cache_list.begin();
}

int LRU::operator[](int key){
    return get(key);
}
