#ifndef LIFEM_H
#define LIFEM_H

#include <string>
#include <vector>

struct Point {
    int x;
    int y;
};

inline constexpr int FIELD_SIDE = 20;
inline constexpr int RULE_SIZE = 9;

const std::string DEFAULT_NAME = "Conway's game of life";
const std::string DEFAULT_BIRTH = "3";
const std::string DEFAULT_SURVIVE = "23";

const std::vector<Point> DEFAULT_CELLS = {
    {1, 2}, {2, 3}, {3, 1}, {3, 2}, {3, 3},
    {10, 10}, {10, 11}, {11, 10}, {11, 11},
    {15, 4}, {15, 5}, {15, 6},
};

class Field {
private:
    int side;
    std::vector<unsigned char> cells;

    int wrap(int coord) const;
    int byteOf(int row, int column) const;
    static int bitOf(int column);

public:
    explicit Field(int size = FIELD_SIDE);
    int getSize() const;
    bool getState(int row, int column) const;
    void setState(int row, int column, bool alive);
    void clear();
};

class LifeModel {
private:
    std::string name;
    std::string birthText;
    std::string surviveText;
    std::vector<bool> birth;
    std::vector<bool> survive;
    Field current;
    Field next;
    int iteration = 0;

    int countNeighbours(int row, int column) const;

public:
    LifeModel();
    LifeModel(
        const std::string& title,
        const std::string& birthRule,
        const std::string& surviveRule,
        const std::vector<Point>& coordinates
    );

    void computeIteration();
    int getIteration() const;
    const std::string& getName() const;
    std::string getRule() const;
    const Field& getField() const;
    std::vector<Point> getAliveCells() const;
};

std::vector<bool> ruleToMask(const std::string& rule);

#endif
