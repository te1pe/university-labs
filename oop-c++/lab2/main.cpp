#include <iostream>
#include "bitarr.h"

static void show(const char* title, const BitArray& b) {
    std::cout << title << ": " << b.to_string() << std::endl;
}

int main() {
    BitArray empty;
    std::cout << "empty.empty(): " << empty.empty() << std::endl;
    std::cout << "empty.size(): " << empty.size() << std::endl;

    BitArray left(10, 5);
    show("left", left);

    left.set(3);
    left.set(7, true);
    left.set(0, false);
    show("after set", left);

    std::cout << "left[3]: " << left[3] << std::endl;
    left[3] = false;
    show("after left[3] = false", left);

    BitArray right(10, 3);
    show("right", right);

    show("left & right", left & right);
    show("left | right", left | right);
    show("left ^ right", left ^ right);
    show("left << 2", left << 2);
    show("left >> 2", left >> 2);
    show("~left", ~left);

    std::cout << "left.any(): " << left.any() << std::endl;
    std::cout << "left.none(): " << left.none() << std::endl;
    std::cout << "left.count(): " << left.count() << std::endl;

    left.push_back(true);
    left.push_back(false);
    std::cout << "after push_back, size: " << left.size() << std::endl;
    show("left", left);

    left.resize(15, true);
    show("after resize(15, true)", left);

    left.swap(right);
    show("after swap, left", left);
    show("after swap, right", right);

    left.clear();
    std::cout << "after clear, left.empty(): " << left.empty() << std::endl;

    try {
        BitArray broken(4);
        broken.set(10);
    } catch (const std::out_of_range& e) {
        std::cout << "caught: " << e.what() << std::endl;
    }

    try {
        BitArray a(4);
        BitArray b(8);
        a &= b;
    } catch (const std::invalid_argument& e) {
        std::cout << "caught: " << e.what() << std::endl;
    }

    return 0;
}
