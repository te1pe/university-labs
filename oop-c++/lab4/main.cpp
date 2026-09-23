#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include "configreader.h"
#include "wavr.h"
#include "wavw.h"
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage:\n"
                  << "  " << argv[0] << " -h                     Show help\n"
                  << "  " << argv[0] << " -c config.txt output.wav input1.wav [input2.wav ...]\n";
        return 1;
    }

    std::string arg1 = argv[1];
    if (arg1 == "-h") {
        ConfigReader reader;
        std::cout << reader.get_help();
        return 0;
    }

    if (arg1 != "-c" || argc < 5) {
        std::cerr << "Invalid arguments. Use -h for help.\n";
        return 1;
    }

    std::string config_file = argv[2];
    std::string output_file = argv[3];

    try {
        std::vector<std::vector<int16_t>> inputs;
        for (int i = 4; i < argc; i++) {
            std::cout << "Reading: " << argv[i] << "\n";
            inputs.push_back(wav_reader::read_wav(argv[i]));
        }

        if (inputs.empty()) {
            std::cerr << "No input files provided\n";
            return 1;
        }

        std::vector<std::string> input_files;
        for (int i = 4; i < argc; i++) {
            input_files.emplace_back(argv[i]);
        }

        ConfigReader reader;
        auto converters = reader.parser(config_file, input_files);

        std::cout << "Loaded " << converters.size() << " converters\n";
        std::vector<int16_t> result = inputs[0];
        for (size_t i = 0; i < converters.size(); i++) {
            std::cout << "Applying converter " << (i + 1) << "...\n";
            converters[i]->process(result, inputs);
        }

        std::cout << "Writing output: " << output_file << "\n";
        wav_writer::write(output_file, result);
        std::cout << "Successfully processed audio!\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
