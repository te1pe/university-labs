#include <iostream>
#include "csv.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "wrong input" << std::endl;
        return 1;
    }

    CSV programm;
    if (!programm.OpenFIle(argv[1])) {
        return 1;
    }

    if (!programm.CloseFile("output.csv")) {
        return 1;
    }

    return 0;
}
