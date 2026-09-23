#ifndef BITARR_H
#define BITARR_H

#include <vector>
#include <string>
#include <stdexcept>
#include <climits>

class BitArray {
private:
    typedef unsigned long Block;
    static const int BITS_IN_BLOCK = sizeof(Block) * CHAR_BIT;

    std::vector<Block> blocks;
    int bit_count;

    static int blocks_for(int bits);
    void check_index(int i) const;
    void trim_tail();

public:
    BitArray();
    ~BitArray();

    explicit BitArray(int num_bits, unsigned long value = 0);
    BitArray(const BitArray& b);
    BitArray& operator=(const BitArray& b);

    void swap(BitArray& b);
    void resize(int num_bits, bool value = false);
    void clear();
    void push_back(bool bit);

    BitArray& operator&=(const BitArray& b);
    BitArray& operator|=(const BitArray& b);
    BitArray& operator^=(const BitArray& b);

    BitArray& operator<<=(int n);
    BitArray& operator>>=(int n);
    BitArray operator<<(int n) const;
    BitArray operator>>(int n) const;

    BitArray& set(int n, bool val = true);
    BitArray& set();
    BitArray& reset(int n);
    BitArray& reset();

    bool any() const;
    bool none() const;
    BitArray operator~() const;
    int count() const;

    class reference {
    private:
        BitArray& owner;
        int index;
    public:
        reference(BitArray& array, int i);
        reference& operator=(bool x);
        reference& operator=(const reference& x);
        operator bool() const;
        reference& flip();
    };

    reference operator[](int i);
    bool operator[](int i) const;

    int size() const;
    bool empty() const;
    std::string to_string() const;

    friend bool operator==(const BitArray& a, const BitArray& b);
    friend bool operator!=(const BitArray& a, const BitArray& b);
    friend BitArray operator&(const BitArray& b1, const BitArray& b2);
    friend BitArray operator|(const BitArray& b1, const BitArray& b2);
    friend BitArray operator^(const BitArray& b1, const BitArray& b2);
};

#endif
