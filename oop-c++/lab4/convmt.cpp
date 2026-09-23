#include "conv.h"
#include <stdexcept>
#include "convmt.h"

void MuteConverter::set_parameters(const std::vector<std::string> &params, const std::vector<std::string> &input_files) {
    if (params.size() < 2) {
        throw std::invalid_argument("Mute converter requires 2 parameters: start_time end_time");
    }

    float start_sec = std::stof(params[0]);
    float end_sec = std::stof(params[1]);

    start_sample = static_cast<size_t>(start_sec * 44100);
    end_sample = static_cast<size_t>(end_sec * 44100);
}

void MuteConverter::process(std::vector<int16_t> &main_stream, const std::vector<std::vector<int16_t>> &all_streams) {
    if (start_sample >= main_stream.size()) {
        return;
    }

    size_t actual_end = std::min(end_sample, main_stream.size());
    for (size_t i = start_sample; i < actual_end; i++) {
        main_stream[i] = 0;
    }
}

std::string MuteConverter::get_description() const {
    return "mute <start_sec> <end_sec> - заглушение интервала времени";
}
