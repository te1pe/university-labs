#pragma once
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
class wav_reader {
private:
public:
    static std::vector<int16_t> read_wav(const std::string& filename);
    static std::vector<std::vector<int16_t>> load_all_inputs(int argc, char* argv[]);
};
