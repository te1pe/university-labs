#include <iostream>
#include "ICACHELRU.hpp"
#include "ICACHELRU.hpp"
#include "fibonachi.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0]
                  << " <cache_size> <LRU|LFU>\n";
        return 1;
    }
    int cache_size = std::atoi(argv[1]);
    if (cache_size < 1) {
        std::cout << "Error: cache_size must be >= 1\n";
        return 1;
    }
    std::string mode = argv[2];
    if (mode == "LRU") {
        LRU cache(cache_size);
        std::cout << "Mode: LRU\n";
        std::cout << "Type '0' for exit\n";
        int n = 0;
        while (true) {
            if (!(std::cin >> n)) break;
            if (n == 0) return 0;
            n = fibonachi(n, cache);
            std::cout << n << '\n';
        }
    } else if (mode == "LFU") {
        LFU cache(cache_size);
        std::cout << "Mode: LFU\n";
        std::cout << "Type '0' for exit\n";
        int n = 0;
        while (true) {
            if (!(std::cin >> n)) break;
            if (n == 0) return 0;
            n = fibonachi(n, cache);
            std::cout << n << '\n';
        }
    } else {
        std::cout << "Error: unknown mode '" << mode
                  << "'. Use LRU or LFU.\n";
        return 1;
    }
    return 0;
}
