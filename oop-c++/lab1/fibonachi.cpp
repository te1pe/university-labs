#include "fibonachi.hpp"


int fibonachi(int n, Icache& cache) {
    if(n == 1 || n == 2)return 1;
    int value = cache[n];
    if (value == -1) {
        value = fibonachi(n - 1, cache) + fibonachi(n - 2, cache);
        cache.put(n, value);
        return value;
    }
    return value;
}
