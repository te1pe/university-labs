#include "conv.h"
#include <stdexcept>
#include "convl.h"

void VolumeConverter::set_parameters(const std::vector<std::string> &params, const std::vector<std::string> &input_files) {
    if (params.empty()) {
        throw std::invalid_argument("Volume converter requires volume factor");
    }
    volume_factor = std::stof(params[0]);
}

void VolumeConverter::process(std::vector<int16_t> &main_stream, const std::vector<std::vector<int16_t>> &all_streams) {
    for (auto& sample : main_stream) {
        float scaled = static_cast<float>(sample) * volume_factor;

        if (scaled > 32767.0f){
            scaled = 32767.0f;
        }

        if (scaled < -32768.0f){
            scaled = -32768.0f;
        }

        sample = static_cast<int16_t>(scaled);
    }
}

std::string VolumeConverter::get_description() const {
    return "volume <factor> - изменяет громкость, домножая текущую на кэфчик";
}
