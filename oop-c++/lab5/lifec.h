#ifndef LIFEC_H
#define LIFEC_H

#include "lifem.h"
#include "lifev.h"
#include <memory>
#include <string>
#include <vector>

class LifeController {
private:
    std::unique_ptr<LifeModel> model;
    std::string outputFile;
    int offlineTicks = 0;

    static std::vector<std::string> splitTokens(const std::string& line);
    static int parseNumber(const std::string& text);
    static bool readRule(const std::string& text, std::string& birth, std::string& survive);

    void loadUniverse(const std::string& path);
    void saveUniverse(const std::string& path) const;
    void runOffline();
    void runInteractive();

public:
    LifeController();
    explicit LifeController(const std::string& inputFile);
    LifeController(const std::string& inputFile, const std::string& outFile, const std::string& ticks);

    void run();
};

#endif
