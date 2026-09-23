#pragma once
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

class wav_writer {
public:
    static void write(const std::string& filename, const std::vector<int16_t>& samples);
};
