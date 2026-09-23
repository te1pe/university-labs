#include "lifec.h"
#include <cctype>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>

/*
Разбор файла делит ошибки на две группы. Критические — файла нет, заголовок
не тот, выходной файл не открывается: без них игры не будет, поэтому летит
std::invalid_argument. Некритические — нет имени, нет правила, повторяются
координаты, мусор в строке: о них печатается предупреждение, строка
пропускается, вселенная всё равно запускается.
*/

std::vector<std::string> LifeController::splitTokens(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

int LifeController::parseNumber(const std::string& text) {
    if (text.empty()) {
        throw std::invalid_argument("number expected, got an empty string");
    }
    size_t start = (text[0] == '-' || text[0] == '+') ? 1 : 0;
    if (start == text.size()) {
        throw std::invalid_argument("'" + text + "' is not a number");
    }
    for (size_t i = start; i < text.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
            throw std::invalid_argument("'" + text + "' is not a number");
        }
    }
    return std::stoi(text);
}

// принимает и "B3/S23", и "B3 S23"; набор цифр может быть пустым
bool LifeController::readRule(const std::string& text, std::string& birth, std::string& survive) {
    std::string cleaned;
    for (char symbol : text) {
        cleaned.push_back(symbol == '/' ? ' ' : symbol);
    }

    std::vector<std::string> parts = splitTokens(cleaned);
    if (parts.size() != 2) {
        return false;
    }

    std::string birthDigits;
    std::string surviveDigits;
    bool hasBirth = false;
    bool hasSurvive = false;

    for (const std::string& part : parts) {
        std::string digits = part.substr(1);
        for (char digit : digits) {
            if (digit < '0' || digit > '8') {
                return false;
            }
        }
        if (part[0] == 'B' || part[0] == 'b') {
            birthDigits = digits;
            hasBirth = true;
        } else if (part[0] == 'S' || part[0] == 's') {
            surviveDigits = digits;
            hasSurvive = true;
        } else {
            return false;
        }
    }

    if (!hasBirth || !hasSurvive) {
        return false;
    }
    birth = birthDigits;
    survive = surviveDigits;
    return true;
}

void LifeController::loadUniverse(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::invalid_argument("cannot open universe file: " + path);
    }

    std::string line;
    if (!std::getline(file, line) || line.rfind("#Life 1.06", 0) != 0) {
        throw std::invalid_argument("file is not in Life 1.06 format: " + path);
    }

    std::string name = DEFAULT_NAME;
    std::string birth = DEFAULT_BIRTH;
    std::string survive = DEFAULT_SURVIVE;
    std::vector<Point> coordinates;
    std::set<std::pair<int, int>> seen;
    bool hasName = false;
    bool hasRule = false;
    int number = 1;

    while (std::getline(file, line)) {
        ++number;
        std::string where = "line " + std::to_string(number) + ": ";

        if (splitTokens(line).empty()) {
            continue;
        }
        if (line.rfind("#N ", 0) == 0) {
            name = line.substr(3);
            hasName = true;
            continue;
        }
        if (line.rfind("#R ", 0) == 0) {
            hasRule = readRule(line.substr(3), birth, survive);
            if (!hasRule) {
                LifeView::printWarning(where + "transition rule is broken, default one is used");
            }
            continue;
        }
        if (line[0] == '#') {
            continue;
        }

        std::vector<std::string> tokens = splitTokens(line);
        if (tokens.size() != 2) {
            LifeView::printWarning(where + "two coordinates expected, line skipped");
            continue;
        }
        try {
            Point cell = {parseNumber(tokens[0]), parseNumber(tokens[1])};
            if (!seen.insert({cell.x, cell.y}).second) {
                LifeView::printWarning(where + "cell is listed twice, line skipped");
                continue;
            }
            coordinates.push_back(cell);
        } catch (const std::invalid_argument& error) {
            LifeView::printWarning(where + error.what() + ", line skipped");
        }
    }

    if (!hasName) {
        LifeView::printWarning("universe has no name, default one is used");
    }
    if (!hasRule) {
        LifeView::printWarning("universe has no transition rule, B" + DEFAULT_BIRTH + "/S" + DEFAULT_SURVIVE + " is used");
    }
    if (coordinates.empty()) {
        LifeView::printWarning("universe has no alive cells, the field will stay empty");
    }

    model = std::make_unique<LifeModel>(name, birth, survive, coordinates);
}

void LifeController::saveUniverse(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::invalid_argument("cannot open output file: " + path);
    }

    file << "#Life 1.06" << std::endl;
    file << "#N " << model->getName() << std::endl;
    file << "#R " << model->getRule() << std::endl;
    for (const Point& cell : model->getAliveCells()) {
        file << cell.x << " " << cell.y << std::endl;
    }
}

LifeController::LifeController() {
    model = std::make_unique<LifeModel>();
}

LifeController::LifeController(const std::string& inputFile) {
    loadUniverse(inputFile);
}

LifeController::LifeController(
    const std::string& inputFile,
    const std::string& outFile,
    const std::string& ticks
) {
    offlineTicks = parseNumber(ticks);
    if (offlineTicks < 0) {
        throw std::invalid_argument("number of iterations cannot be negative");
    }
    outputFile = outFile;
    loadUniverse(inputFile);
}

void LifeController::runOffline() {
    for (int i = 0; i < offlineTicks; ++i) {
        model->computeIteration();
    }
    saveUniverse(outputFile);
    std::cout << "Universe after " << offlineTicks
              << " iterations is saved to " << outputFile << std::endl;
}

void LifeController::runInteractive() {
    LifeView::printFrame(*model, 0, 0);
    LifeView::printHelp();

    std::string line;
    while (std::getline(std::cin, line)) {
        std::vector<std::string> tokens = splitTokens(line);
        if (tokens.empty()) {
            continue;
        }
        const std::string& command = tokens[0];

        if (command == "exit") {
            return;
        }

        if (command == "help") {
            LifeView::printHelp();
        } else if (command == "tick" || command == "t") {
            int ticks = 1;
            if (tokens.size() > 1) {
                try {
                    ticks = parseNumber(tokens[1]);
                } catch (const std::invalid_argument& error) {
                    LifeView::printWarning(std::string(error.what()) + ", one iteration is done");
                    ticks = 1;
                }
            }
            if (ticks < 1) {
                LifeView::printWarning("number of iterations must be positive, one iteration is done");
                ticks = 1;
            }
            for (int step = 1; step <= ticks; ++step) {
                model->computeIteration();
                LifeView::printFrame(*model, step, ticks);
            }
            LifeView::printHelp();
        } else if (command == "dump") {
            if (tokens.size() < 2) {
                LifeView::printError("dump needs a file name");
            } else {
                try {
                    saveUniverse(tokens[1]);
                    std::cout << "Universe is saved to " << tokens[1] << std::endl;
                } catch (const std::invalid_argument& error) {
                    LifeView::printError(error.what());
                }
            }
        } else {
            LifeView::printError("unknown command: " + command);
            LifeView::printHelp();
        }
    }
}

void LifeController::run() {
    if (outputFile.empty()) {
        runInteractive();
    } else {
        runOffline();
    }
}
