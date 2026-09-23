#include "lifem.h"
#include <algorithm>
#include <utility>

/*
Поле замкнуто в тор: сосед справа от последнего столбца — первый столбец,
сосед сверху от первого ряда — последний ряд. Поэтому wrap() приводит любую
координату к диапазону [0, side) и getState/setState принимают отрицательные
индексы — вызывающему коду не нужно думать о краях.

Клетки упакованы по 8 штук в байт: строка занимает (side + 7) / 8 байт,
поле целиком — side * bytesInRow байт.
*/

Field::Field(int size) : side(size) {
    cells.assign(side * ((side + 7) / 8), 0);
}

int Field::wrap(int coord) const {
    coord %= side;
    return coord < 0 ? coord + side : coord;
}

int Field::byteOf(int row, int column) const {
    return row * ((side + 7) / 8) + column / 8;
}

int Field::bitOf(int column) {
    return column % 8;
}

int Field::getSize() const {
    return side;
}

bool Field::getState(int row, int column) const {
    int r = wrap(row);
    int c = wrap(column);
    return (cells[byteOf(r, c)] & (1 << bitOf(c))) != 0;
}

void Field::setState(int row, int column, bool alive) {
    int r = wrap(row);
    int c = wrap(column);
    unsigned char mask = static_cast<unsigned char>(1 << bitOf(c));
    if (alive) {
        cells[byteOf(r, c)] |= mask;
    } else {
        cells[byteOf(r, c)] &= static_cast<unsigned char>(~mask);
    }
}

void Field::clear() {
    std::fill(cells.begin(), cells.end(), 0);
}

// "23" -> [false, false, true, true, false, false, false, false, false]
std::vector<bool> ruleToMask(const std::string& rule) {
    std::vector<bool> mask(RULE_SIZE, false);
    for (char digit : rule) {
        if (digit >= '0' && digit <= '8') {
            mask[digit - '0'] = true;
        }
    }
    return mask;
}

LifeModel::LifeModel() : LifeModel(
    DEFAULT_NAME,
    DEFAULT_BIRTH,
    DEFAULT_SURVIVE,
    DEFAULT_CELLS
) {}

LifeModel::LifeModel(
    const std::string& title,
    const std::string& birthRule,
    const std::string& surviveRule,
    const std::vector<Point>& coordinates
) :
    name(title),
    birthText(birthRule),
    surviveText(surviveRule),
    birth(ruleToMask(birthRule)),
    survive(ruleToMask(surviveRule)),
    current(FIELD_SIDE),
    next(FIELD_SIDE)
{
    for (const Point& cell : coordinates) {
        current.setState(cell.y, cell.x, true);
    }
}

int LifeModel::countNeighbours(int row, int column) const {
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            if (current.getState(row + dr, column + dc)) {
                ++count;
            }
        }
    }
    return count;
}

void LifeModel::computeIteration() {
    for (int row = 0; row < current.getSize(); ++row) {
        for (int column = 0; column < current.getSize(); ++column) {
            int neighbours = countNeighbours(row, column);
            bool alive = current.getState(row, column);
            next.setState(row, column, alive ? survive[neighbours] : birth[neighbours]);
        }
    }
    std::swap(current, next);
    ++iteration;
}

int LifeModel::getIteration() const {
    return iteration;
}

const std::string& LifeModel::getName() const {
    return name;
}

std::string LifeModel::getRule() const {
    return "B" + birthText + "/S" + surviveText;
}

const Field& LifeModel::getField() const {
    return current;
}

std::vector<Point> LifeModel::getAliveCells() const {
    std::vector<Point> alive;
    for (int row = 0; row < current.getSize(); ++row) {
        for (int column = 0; column < current.getSize(); ++column) {
            if (current.getState(row, column)) {
                alive.push_back({column, row});
            }
        }
    }
    return alive;
}
