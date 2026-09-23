#pragma once
#include "conv.h"
#include "convf.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

using ConverterPtr = std::unique_ptr<Converter>;

class ConfigReader {
private:
    ConverterFactory factory;
    std::vector<std::string> split_line(const std::string& line);

public:
    std::vector<ConverterPtr> parser(const std::string& config_file, const std::vector<std::string>& input_files);
    std::string get_help() const;

};
