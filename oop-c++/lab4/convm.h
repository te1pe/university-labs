#pragma once
#include "conv.h"
#include <stdexcept>
#include <string>

class MixConverter : public Converter {
private:
    size_t stream_index = 0;
    size_t start_sample = 0;

    size_t parse_stream_reference(const std::string& ref);

public:
    void set_parameters(const std::vector<std::string>& params, const std::vector<std::string>& input_files) override;
    void process(std::vector<int16_t>& main_stream, const std::vector<std::vector<int16_t>>& all_streams) override;
    std::string get_description() const override;
};
