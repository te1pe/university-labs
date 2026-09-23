#ifndef LIFEV_H
#define LIFEV_H

#include "lifem.h"
#include <iostream>
#include <string>

class LifeView {
public:
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printHelp();
    static void printFrame(const LifeModel& model, int step, int total);
};

#endif
