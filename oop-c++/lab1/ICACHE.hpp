#ifndef ICACHE_HPP
#define ICACHE_HPP

#include<iostream>
#include<list>
#include<unordered_map>
#include<functional>
#include<map>

class Icache{
    public:
        virtual int get(int key)=0;
        virtual void put(int key, int data)=0;
        virtual int operator[](int key) = 0;
};

#endif 
