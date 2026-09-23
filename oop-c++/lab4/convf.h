#pragma once
#include "conv.h"
#include "convm.h"
#include "convmt.h"
#include "convl.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <stdexcept>

using ConverterPtr = std::unique_ptr<Converter>;

class ConverterFactory {
private:
    std::unordered_map<std::string, std::function<ConverterPtr()>> creators;
public:
    ConverterFactory();
    void register_converters();
    ConverterPtr create(const std::string& name) const;
    std::vector<std::string> get_available_converters() const;
};
