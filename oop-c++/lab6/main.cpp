#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "csvparser.h"
#include "tupleout.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: " << argv[0] << " <file.csv> [skip_lines]\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cout << "mistake open " << argv[1] << std::endl;
        return 1;
    }

    size_t skipLines = 0;
    if (argc == 3) {
        skipLines = static_cast<size_t>(std::atoi(argv[2]));
    }

    try {
        CSVParser<int, std::string, double> parser(file, skipLines);
        for (const std::tuple<int, std::string, double>& row : parser) {
            std::cout << row << std::endl;
        }
    } catch (const CSVError& error) {
        std::cerr << "Parse error at " << error.what() << "\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
