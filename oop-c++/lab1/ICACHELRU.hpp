#ifndef ICACHELRU_HPP
#define ICACHELRU_HPP

#include "ICACHE.hpp"


class LRU: public Icache{
    private:
        std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache_map;
        std::list<std::pair<int, int>> cache_list;
        const size_t cache_size;
    public:
        explicit LRU(size_t n);
        int get(int key) override;
        void put(int key, int data) override;
        int operator[](int key) override; 
};


#endif
