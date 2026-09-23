#ifndef ICACHELFU_HPP
#define ICACHELFU_HPP

#include "ICACHE.hpp"

class LFU : public Icache {
private:
    struct Node {
        int key;
        int data;
        int freq;
    };
    const size_t cache_size;
    std::unordered_map<int, std::list<Node>::iterator> nodes;
    std::unordered_map<int, std::list<Node>> freq_lists;
    int min_freq = 0;
    using NodesMapIt = std::unordered_map<int, std::list<Node>::iterator>::iterator;
    void touch(NodesMapIt it);
public:
    explicit LFU(size_t n);
    int get(int key) override;
    void put(int key, int data) override;
    int operator[](int key) override;
};

#endif 
