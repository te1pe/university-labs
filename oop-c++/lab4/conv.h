#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

class Converter {
public:
    virtual std::string get_description() const = 0;
    virtual void process(std::vector<int16_t>& main_stream, const std::vector<std::vector<int16_t>>& all_streams) = 0;
    virtual void set_parameters(const std::vector<std::string>& params, const std::vector<std::string>& input_files) = 0;
};
