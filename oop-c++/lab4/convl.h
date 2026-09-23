#pragma once
#include "conv.h"
#include <stdexcept>

class VolumeConverter : public Converter {
private:
    float volume_factor = 1.0f;
public:
    void set_parameters(const std::vector<std::string>& params, const std::vector<std::string>& input_files) override;
    void process(std::vector<int16_t>& main_stream, const std::vector<std::vector<int16_t>>& all_streams) override;
    std::string get_description() const override;
};
