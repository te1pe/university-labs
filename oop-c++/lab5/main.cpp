#include <iostream>
#include <string>
#include <vector>
#include "lifec.h"

static void printUsage(const char* program) {
    std::cout << "Usage:\n"
              << "  " << program << "                                     запуск на встроенной вселенной\n"
              << "  " << program << " <universe.life>                     запуск на вселенной из файла\n"
              << "  " << program << " --input <in> --output <out> --iterations <n>\n"
              << "                                             оффлайн-режим, короткие ключи -I, -o, -i\n";
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);

    try {
        if (argc == 1) {
            LifeController game;
            game.run();
            return 0;
        }

        if (argc == 2) {
            if (args[1] == "-h" || args[1] == "--help") {
                printUsage(argv[0]);
                return 0;
            }
            LifeController game(args[1]);
            game.run();
            return 0;
        }

        std::string input;
        std::string output;
        std::string iterations;

        for (int i = 1; i < argc; i += 2) {
            const std::string& key = args[i];
            if (i + 1 >= argc) {
                std::cout << "Missing value for " << key << "\n";
                printUsage(argv[0]);
                return 1;
            }
            const std::string& value = args[i + 1];

            if (key == "--input" || key == "-I") {
                input = value;
            } else if (key == "--output" || key == "-o") {
                output = value;
            } else if (key == "--iterations" || key == "-i") {
                iterations = value;
            } else {
                std::cout << "Unknown key " << key << "\n";
                printUsage(argv[0]);
                return 1;
            }
        }

        if (input.empty() || output.empty() || iterations.empty()) {
            std::cout << "Offline mode needs input, output and iterations\n";
            printUsage(argv[0]);
            return 1;
        }

        LifeController game(input, output, iterations);
        game.run();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
