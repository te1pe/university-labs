#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include "wavw.h"

void wav_writer::write(const std::string &filename, const std::vector<int16_t> &samples) {
    std::ofstream file(filename, std::ios::binary);
    unsigned char header[44] = {
            'R', 'I', 'F', 'F',
            0, 0, 0, 0,
            'W', 'A', 'V', 'E',
            'f', 'm', 't', ' ',
            16, 0, 0, 0,
            1, 0,
            1, 0,
            0x44, 0xAC, 0, 0,
            0x88, 0x58, 0x1, 0,
            2, 0,
            16, 0,
            'd', 'a', 't', 'a',
            0, 0, 0, 0
    };

    uint32_t data_size = samples.size() * 2;
    uint32_t file_size = data_size + 36;

    header[4] = file_size & 0xFF;
    header[5] = (file_size >> 8) & 0xFF;
    header[6] = (file_size >> 16) & 0xFF;
    header[7] = (file_size >> 24) & 0xFF;

    header[40] = data_size & 0xFF;
    header[41] = (data_size >> 8) & 0xFF;
    header[42] = (data_size >> 16) & 0xFF;
    header[43] = (data_size >> 24) & 0xFF;

    file.write(reinterpret_cast<char*>(header), 44);
    file.write(reinterpret_cast<const char*>(samples.data()), data_size);
}
