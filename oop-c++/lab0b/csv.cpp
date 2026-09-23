#include "csv.h"
#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>
#include <cctype>
#include <vector>
#include <algorithm>

CSV::CSV() : totalWords(0), currentWord("") {}

bool CSV::Isword(unsigned char c) {
    if (std::isalnum(c)) return true;
    if (c == '\'' || c == '.' || c == '-') return true;
    if ((c >= 128)) return true;
    return false;
}
/*
128 мы берем для проверки символов, в аски все символы проверяются 

if (std::isalnum(c)) return true; 
if (c == '\'' || c == '.' || c == '-') return true; 

остальные же символы будь то это кирилици или иероглифы кодируются страршим байтом = 1
поэтому и будет смотреть >=128
    

*/ 
std::string CSV::PointDefis(const std::string& word) {
    if (word.empty()) return word;
    std::string result = word;
    while (!result.empty() && (result.back() == '.' || result.back() == '-')) {
        result.pop_back();
    }
    return result;
}

bool CSV::ComparePairs(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    return a.second > b.second;
}

bool CSV::OpenFIle(const char* filename) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cout << "mistake open" << std::endl;
        return false;
    }

    char* inputBuffer = new char[BUFFER];
    inputFile.rdbuf()->pubsetbuf(inputBuffer, BUFFER);

    char c;
    while(inputFile.get(c)) {
        if (Isword(static_cast<unsigned char>(c))) {
            currentWord += std::tolower(static_cast<unsigned char>(c));
        }
        else if (!currentWord.empty()) {
            std::string cleanedWord = PointDefis(currentWord);
            if (!cleanedWord.empty()) {
                wordCount[cleanedWord]++;
                totalWords++;
            }
            currentWord = "";
        }
    }

    if (!currentWord.empty()) {
        std::string cleanedWord = PointDefis(currentWord);
        if (!cleanedWord.empty()) {
            wordCount[cleanedWord]++;
            totalWords++;
        }
    }

    inputFile.close();
    delete[] inputBuffer;
    return true;
}

bool CSV::CloseFile(const char* filename) {
    std::vector<std::pair<std::string, int>> wordList;
    for (auto& pair : wordCount) {
        wordList.push_back(std::make_pair(pair.first, pair.second));
    }

    std::sort(wordList.begin(), wordList.end(), ComparePairs);

    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cout << "mistake open ";
        return false;
    }

    char* outputBuffer = new char[BUFFER];
    outputFile.rdbuf()->pubsetbuf(outputBuffer, BUFFER);

    outputFile << "Word,Count,Frequency" << std::endl;

    for (auto& pair: wordList) {
        float percent = (pair.second * 100.0f) / totalWords;
        outputFile << pair.first << "," << pair.second << "," 
                  << std::fixed << std::setprecision(6) << percent << "%" << std::endl;
    }

    outputFile.close();
    delete[] outputBuffer;
    return true;
}
