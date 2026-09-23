#include <iostream>
#include <chrono>

#define MAX_BLOCKS 16
#define NUM_OF_RUNS 8
#define WARMUP_ITERS 10000
#define MEASURE_ITERS 50000

extern "C" void func0() __attribute__((noinline));
extern "C" void func1() __attribute__((noinline));
extern "C" void func2() __attribute__((noinline));
extern "C" void func3() __attribute__((noinline));
extern "C" void func4() __attribute__((noinline));
extern "C" void func5() __attribute__((noinline));
extern "C" void func6() __attribute__((noinline));
extern "C" void func7() __attribute__((noinline));
extern "C" void func8() __attribute__((noinline));
extern "C" void func9() __attribute__((noinline));
extern "C" void func10() __attribute__((noinline));
extern "C" void func11() __attribute__((noinline));
extern "C" void func12() __attribute__((noinline));
extern "C" void func13() __attribute__((noinline));
extern "C" void func14() __attribute__((noinline));
extern "C" void func15() __attribute__((noinline));

asm(
    ".text\n"

    ".p2align 12\n"
    ".global func0\n"
    "func0:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func1\n"
    "func1:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func2\n"
    "func2:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func3\n"
    "func3:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func4\n"
    "func4:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func5\n"
    "func5:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func6\n"
    "func6:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func7\n"
    "func7:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func8\n"
    "func8:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func9\n"
    "func9:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func10\n"
    "func10:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func11\n"
    "func11:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func12\n"
    "func12:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func13\n"
    "func13:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func14\n"
    "func14:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
    ".p2align 12\n"
    ".global func15\n"
    "func15:\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
    "ret\n"
);

using FuncPtr = void (*)();

static FuncPtr funcs[MAX_BLOCKS] = {
    func0, func1, func2, func3,
    func4, func5, func6, func7,
    func8, func9, func10, func11,
    func12, func13, func14, func15
};

long long InstructionTime(int blocks_num) {
    using clock = std::chrono::high_resolution_clock;

    for (int r = 0; r < WARMUP_ITERS; r++) {
        for (int i = 0; i < blocks_num; i++) {
            funcs[i]();
        }
    }

    auto start = clock::now();

    for (int r = 0; r < MEASURE_ITERS; r++) {
        for (int i = 0; i < blocks_num; i++) {
            funcs[i]();
        }
    }

    auto end = clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    long long total_calls = (long long)MEASURE_ITERS * blocks_num;
    long long average_time = duration.count() / total_calls;

    return average_time;
}

int main() {
    for (int blocks_num = 1; blocks_num <= MAX_BLOCKS; blocks_num++) {
        long long sum_time = 0;

        for (int i = 0; i < NUM_OF_RUNS; i++) {
            sum_time += InstructionTime(blocks_num);
        }

        std::cout << "Number of blocks: " << blocks_num
                  << "; average time: " << sum_time / NUM_OF_RUNS
                  << " ns\n";
    }

    return 0;
}
