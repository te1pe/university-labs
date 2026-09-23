#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include "wavr.h"

std::vector<int16_t> wav_reader::read_wav(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    file.seekg(44);
    std::vector<int16_t> samples;
    int16_t sample;

    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        samples.push_back(sample);
    }

    return samples;
}
std::vector<std::vector<int16_t>> wav_reader::load_all_inputs(int argc, char **argv) {
    std::vector<std::vector<int16_t>> inputs;
    for (int i = 4; i < argc; i++) {
        inputs.push_back(read_wav(argv[i]));
    }
    return inputs;
}
