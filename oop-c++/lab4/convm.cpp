#include "convm.h"
#include "conv.h"
#include <stdexcept>
#include <string>

size_t MixConverter::parse_stream_reference(const std::string &ref) {
    if (ref.empty() || ref[0] != '$') {
        throw std::invalid_argument("Stream reference must start with $");
    }

    int index = std::stoi(ref.substr(1)) - 1;
    if (index < 0) {
        throw std::invalid_argument("Stream index must be positive");
    }

    return static_cast<size_t>(index);
}

void MixConverter::set_parameters(const std::vector<std::string>& params, const std::vector<std::string>& input_files){
    if (params.size() < 1) {
        throw std::invalid_argument("Mix converter requires at least stream reference");
    }

    stream_index = parse_stream_reference(params[0]);
    if (params.size() > 1) {
        float start_sec = std::stof(params[1]);
        start_sample = static_cast<size_t>(start_sec * 44100);
    }
}

void MixConverter::process(std::vector<int16_t> &main_stream, const std::vector<std::vector<int16_t>> &all_streams) {
    if (stream_index >= all_streams.size()) {
        throw std::out_of_range("Invalid stream index");
    }

    const auto& mix_stream = all_streams[stream_index];
    for (size_t i = 0; i < mix_stream.size(); i++) {
        size_t main_pos = start_sample + i;
        if (main_pos >= main_stream.size()) {
            break;
        }
        int32_t mixed = static_cast<int32_t>(main_stream[main_pos]) + static_cast<int32_t>(mix_stream[i]);
        main_stream[main_pos] = static_cast<int16_t>(mixed / 2);
    }
}

std::string MixConverter::get_description() const {
    return "mix <$stream> [start_sec] - микcирует с другим потоком";
}
