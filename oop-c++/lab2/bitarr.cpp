#include "bitarr.h"

/*
Биты лежат упакованно: один Block = 64 бита, бит с индексом i живёт
в blocks[i / 64] на позиции i % 64. Младший бит блока — это бит с
меньшим индексом, поэтому сдвиг массива влево совпадает с обычным
сдвигом многословного числа влево.

Хвост последнего блока почти всегда длиннее, чем нужно: bit_count не
обязан делиться на 64. Лишние биты обязаны оставаться нулями, иначе
count(), any() и сравнение массивов начнут врать. За это отвечает
trim_tail(), который вызывается после каждой операции над блоками.
*/

int BitArray::blocks_for(int bits) {
    return (bits + BITS_IN_BLOCK - 1) / BITS_IN_BLOCK;
}

void BitArray::trim_tail() {
    int used = bit_count % BITS_IN_BLOCK;
    if (used != 0 && !blocks.empty()) {
        blocks.back() &= (Block(1) << used) - 1;
    }
}

void BitArray::check_index(int i) const {
    if (i < 0 || i >= bit_count) {
        throw std::out_of_range("BitArray index out of range");
    }
}

BitArray::BitArray() : bit_count(0) {}

BitArray::~BitArray() {}

BitArray::BitArray(int num_bits, unsigned long value) : bit_count(0) {
    if (num_bits < 0) {
        throw std::invalid_argument("Number of bits cannot be negative");
    }
    blocks.assign(blocks_for(num_bits), 0);
    bit_count = num_bits;
    if (!blocks.empty()) {
        blocks[0] = value;
        trim_tail();
    }
}

BitArray::BitArray(const BitArray& b) : blocks(b.blocks), bit_count(b.bit_count) {}

BitArray& BitArray::operator=(const BitArray& b) {
    if (this != &b) {
        blocks = b.blocks;
        bit_count = b.bit_count;
    }
    return *this;
}

void BitArray::swap(BitArray& b) {
    blocks.swap(b.blocks);
    std::swap(bit_count, b.bit_count);
}

void BitArray::resize(int num_bits, bool value) {
    if (num_bits < 0) {
        throw std::invalid_argument("Number of bits cannot be negative");
    }
    int old_count = bit_count;
    blocks.resize(blocks_for(num_bits), value ? ~Block(0) : Block(0));
    bit_count = num_bits;

    if (value && num_bits > old_count) {
        int used = old_count % BITS_IN_BLOCK;
        if (used != 0) {
            blocks[old_count / BITS_IN_BLOCK] |= (~Block(0)) << used;
        }
    }
    trim_tail();
}

void BitArray::clear() {
    blocks.clear();
    bit_count = 0;
}

void BitArray::push_back(bool bit) {
    resize(bit_count + 1);
    set(bit_count - 1, bit);
}

BitArray& BitArray::operator&=(const BitArray& b) {
    if (bit_count != b.bit_count) {
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise AND operation");
    }
    for (size_t i = 0; i < blocks.size(); ++i) {
        blocks[i] &= b.blocks[i];
    }
    return *this;
}

BitArray& BitArray::operator|=(const BitArray& b) {
    if (bit_count != b.bit_count) {
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise OR operation");
    }
    for (size_t i = 0; i < blocks.size(); ++i) {
        blocks[i] |= b.blocks[i];
    }
    return *this;
}

BitArray& BitArray::operator^=(const BitArray& b) {
    if (bit_count != b.bit_count) {
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise XOR operation");
    }
    for (size_t i = 0; i < blocks.size(); ++i) {
        blocks[i] ^= b.blocks[i];
    }
    return *this;
}

BitArray& BitArray::operator<<=(int n) {
    if (n < 0) {
        return (*this) >>= -n;
    }
    if (n == 0) {
        return *this;
    }
    if (n >= bit_count) {
        return reset();
    }

    int whole = n / BITS_IN_BLOCK;
    int rest = n % BITS_IN_BLOCK;
    int total = static_cast<int>(blocks.size());

    for (int i = total - 1; i >= 0; --i) {
        Block moved = 0;
        int from = i - whole;
        if (from >= 0) {
            moved = blocks[from] << rest;
            if (rest != 0 && from > 0) {
                moved |= blocks[from - 1] >> (BITS_IN_BLOCK - rest);
            }
        }
        blocks[i] = moved;
    }
    trim_tail();
    return *this;
}

BitArray& BitArray::operator>>=(int n) {
    if (n < 0) {
        return (*this) <<= -n;
    }
    if (n == 0) {
        return *this;
    }
    if (n >= bit_count) {
        return reset();
    }

    int whole = n / BITS_IN_BLOCK;
    int rest = n % BITS_IN_BLOCK;
    int total = static_cast<int>(blocks.size());

    for (int i = 0; i < total; ++i) {
        Block moved = 0;
        int from = i + whole;
        if (from < total) {
            moved = blocks[from] >> rest;
            if (rest != 0 && from + 1 < total) {
                moved |= blocks[from + 1] << (BITS_IN_BLOCK - rest);
            }
        }
        blocks[i] = moved;
    }
    trim_tail();
    return *this;
}

BitArray BitArray::operator<<(int n) const {
    BitArray result(*this);
    result <<= n;
    return result;
}

BitArray BitArray::operator>>(int n) const {
    BitArray result(*this);
    result >>= n;
    return result;
}

BitArray& BitArray::set(int n, bool val) {
    check_index(n);
    Block mask = Block(1) << (n % BITS_IN_BLOCK);
    if (val) {
        blocks[n / BITS_IN_BLOCK] |= mask;
    } else {
        blocks[n / BITS_IN_BLOCK] &= ~mask;
    }
    return *this;
}

BitArray& BitArray::set() {
    for (size_t i = 0; i < blocks.size(); ++i) {
        blocks[i] = ~Block(0);
    }
    trim_tail();
    return *this;
}

BitArray& BitArray::reset(int n) {
    return set(n, false);
}

BitArray& BitArray::reset() {
    for (size_t i = 0; i < blocks.size(); ++i) {
        blocks[i] = 0;
    }
    return *this;
}

bool BitArray::any() const {
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i] != 0) {
            return true;
        }
    }
    return false;
}

bool BitArray::none() const {
    return !any();
}

BitArray BitArray::operator~() const {
    BitArray result(*this);
    for (size_t i = 0; i < result.blocks.size(); ++i) {
        result.blocks[i] = ~result.blocks[i];
    }
    result.trim_tail();
    return result;
}

int BitArray::count() const {
    int total = 0;
    for (size_t i = 0; i < blocks.size(); ++i) {
        Block block = blocks[i];
        // алгоритм Кернигана: block & (block - 1) гасит младшую единицу,
        // поэтому шагов ровно столько, сколько единичных бит в блоке
        while (block != 0) {
            block &= block - 1;
            ++total;
        }
    }
    return total;
}

BitArray::reference BitArray::operator[](int i) {
    check_index(i);
    return reference(*this, i);
}

bool BitArray::operator[](int i) const {
    check_index(i);
    Block mask = Block(1) << (i % BITS_IN_BLOCK);
    return (blocks[i / BITS_IN_BLOCK] & mask) != 0;
}

int BitArray::size() const {
    return bit_count;
}

bool BitArray::empty() const {
    return bit_count == 0;
}

std::string BitArray::to_string() const {
    std::string result;
    result.reserve(bit_count);
    for (int i = 0; i < bit_count; ++i) {
        result.push_back((*this)[i] ? '1' : '0');
    }
    return result;
}

bool operator==(const BitArray& a, const BitArray& b) {
    return a.bit_count == b.bit_count && a.blocks == b.blocks;
}

bool operator!=(const BitArray& a, const BitArray& b) {
    return !(a == b);
}

BitArray operator&(const BitArray& b1, const BitArray& b2) {
    BitArray result(b1);
    result &= b2;
    return result;
}

BitArray operator|(const BitArray& b1, const BitArray& b2) {
    BitArray result(b1);
    result |= b2;
    return result;
}

BitArray operator^(const BitArray& b1, const BitArray& b2) {
    BitArray result(b1);
    result ^= b2;
    return result;
}

BitArray::reference::reference(BitArray& array, int i) : owner(array), index(i) {}

BitArray::reference& BitArray::reference::operator=(bool x) {
    owner.set(index, x);
    return *this;
}

BitArray::reference& BitArray::reference::operator=(const reference& x) {
    owner.set(index, static_cast<bool>(x));
    return *this;
}

BitArray::reference::operator bool() const {
    const BitArray& readonly = owner;
    return readonly[index];
}

BitArray::reference& BitArray::reference::flip() {
    owner.set(index, !static_cast<bool>(*this));
    return *this;
}
