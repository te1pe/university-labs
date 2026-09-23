#include "conv.h"
#include "convf.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include "configreader.h"

std::vector<std::string> ConfigReader::split_line(const std::string &line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;

    while (iss >> token) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::vector<ConverterPtr> ConfigReader::parser(const std::string &config_file, const std::vector<std::string> &input_files) {
    std::vector<ConverterPtr> converters;

    std::ifstream file(config_file);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + config_file);
    }

    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;

        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        if (line.empty()) {
            continue;
        }

        auto tokens = split_line(line);
        if (tokens.empty()) {
            continue;
        }

        try {
            auto converter = factory.create(tokens[0]);

            std::vector<std::string> params(tokens.begin() + 1, tokens.end());
            converter->set_parameters(params, input_files);
            converters.push_back(std::move(converter));
        } catch (const std::exception& e) {
            throw std::runtime_error("Error in config line " + std::to_string(line_num) + ": " + e.what());
        }
    }
    return converters;
}

std::string ConfigReader::get_help() const {
    std::string help = "Available converters:\n";
    for (const auto& name : factory.get_available_converters()) {
        auto converter = factory.create(name);
        help += "  " + converter->get_description() + "\n";
    }
    return help;
}
