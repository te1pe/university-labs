#ifndef CSV_H
#define CSV_H

#include <string>
#include <map>

class CSV {
private:
    std::map<std::string, int> wordCount;
    int totalWords;
    std::string currentWord;
    int BUFFER = 5 * 1024 * 1024;

    bool Isword(unsigned char c);
    std::string PointDefis(const std::string& word);
    static bool ComparePairs(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b);

public:
    CSV();
    bool OpenFIle(const char* filename);
    bool CloseFile(const char* filename);
};

#endif 
