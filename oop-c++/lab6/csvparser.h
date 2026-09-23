#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <algorithm>
#include <cstddef>
#include <istream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

// Ошибка разбора всегда знает, где именно она произошла: номер строки
// в файле и номер колонки в строке, обе нумерации с единицы.
class CSVError : public std::runtime_error {
private:
    size_t line;
    size_t column;

    static std::string describe(size_t line, size_t column, const std::string& reason) {
        return "line " + std::to_string(line) + ", column " + std::to_string(column) + ": " + reason;
    }

public:
    CSVError(size_t lineNumber, size_t columnNumber, const std::string& reason)
        : std::runtime_error(describe(lineNumber, columnNumber, reason)),
          line(lineNumber),
          column(columnNumber) {}

    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
};

/*
Разбор одной строки на ячейки. Ячейка, начинающаяся с символа экранирования,
читается до парной закрывающей кавычки: разделитель внутри неё остаётся
обычным символом. Удвоенная кавычка внутри экранированной ячейки означает
одну кавычку в данных.

    один,"два, всё ещё два","кавычка "" внутри"
    -> {"один", "два, всё ещё два", "кавычка \" внутри"}
*/
inline std::vector<std::string> splitRow(
    const std::string& row,
    char columnDelim,
    char escape,
    size_t lineNumber
) {
    std::vector<std::string> cells;
    std::string cell;
    size_t i = 0;

    while (i <= row.size()) {
        if (i == row.size()) {
            cells.push_back(cell);
            break;
        }

        if (row[i] == escape && cell.empty()) {
            ++i;
            bool closed = false;
            while (i < row.size()) {
                if (row[i] == escape) {
                    if (i + 1 < row.size() && row[i + 1] == escape) {
                        cell.push_back(escape);
                        i += 2;
                        continue;
                    }
                    ++i;
                    closed = true;
                    break;
                }
                cell.push_back(row[i]);
                ++i;
            }
            if (!closed) {
                throw CSVError(lineNumber, cells.size() + 1, "escaped cell is not closed");
            }
            if (i < row.size() && row[i] != columnDelim) {
                throw CSVError(lineNumber, cells.size() + 1, "unexpected text after the closing quote");
            }
            continue;
        }

        if (row[i] == columnDelim) {
            cells.push_back(cell);
            cell.clear();
            ++i;
            continue;
        }

        cell.push_back(row[i]);
        ++i;
    }

    return cells;
}

// Заполнение кортежа: рекурсия по индексу, строковые поля берутся как есть,
// остальные читаются потоком и обязаны быть прочитаны целиком.
template <std::size_t Index = 0, typename... Args>
void fillTuple(std::tuple<Args...>& target, const std::vector<std::string>& cells, size_t lineNumber) {
    if constexpr (Index < sizeof...(Args)) {
        using Cell = std::tuple_element_t<Index, std::tuple<Args...>>;
        const std::string& text = cells[Index];

        if constexpr (std::is_same_v<Cell, std::string>) {
            std::get<Index>(target) = text;
        } else {
            std::istringstream stream(text);
            stream >> std::get<Index>(target);
            if (stream.fail() || !(stream >> std::ws).eof()) {
                throw CSVError(lineNumber, Index + 1, "'" + text + "' does not fit the column type");
            }
        }
        fillTuple<Index + 1>(target, cells, lineNumber);
    }
}

template <typename... Args>
class CSVParser {
private:
    std::istream& input;
    size_t skipLines;
    char columnDelim;
    char rowDelim;
    char escape;

public:
    using value_type = std::tuple<Args...>;

    explicit CSVParser(
        std::istream& stream,
        size_t linesToSkip = 0,
        char column = ',',
        char row = '\n',
        char escapeSymbol = '"'
    ) :
        input(stream),
        skipLines(linesToSkip),
        columnDelim(column),
        rowDelim(row),
        escape(escapeSymbol)
    {
        if (!input) {
            throw std::runtime_error("CSV stream is not readable");
        }
    }

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::tuple<Args...>;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        Iterator() = default;

        explicit Iterator(CSVParser& parser) : owner(&parser) {
            std::string skipped;
            for (size_t i = 0; i < owner->skipLines; ++i) {
                if (!std::getline(owner->input, skipped, owner->rowDelim)) {
                    owner = nullptr;
                    return;
                }
                ++lineNumber;
            }
            readRow();
        }

        reference operator*() const { return current; }
        pointer operator->() const { return &current; }

        Iterator& operator++() {
            if (owner != nullptr) {
                readRow();
            }
            return *this;
        }

        bool operator==(const Iterator& other) const { return owner == other.owner; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
        CSVParser* owner = nullptr;
        value_type current;
        size_t lineNumber = 0;

        void readRow() {
            std::string row;
            if (!std::getline(owner->input, row, owner->rowDelim)) {
                owner = nullptr;
                return;
            }
            ++lineNumber;

            // файлы с окончаниями строк CRLF оставляют \r в конце строки
            if (!row.empty() && row.back() == '\r') {
                row.pop_back();
            }

            std::vector<std::string> cells = splitRow(row, owner->columnDelim, owner->escape, lineNumber);
            if (cells.size() != sizeof...(Args)) {
                size_t where = std::min(cells.size(), sizeof...(Args)) + 1;
                throw CSVError(lineNumber, where,
                               "expected " + std::to_string(sizeof...(Args)) +
                               " columns, got " + std::to_string(cells.size()));
            }
            fillTuple(current, cells, lineNumber);
        }
    };

    Iterator begin() { return Iterator(*this); }
    Iterator end() { return Iterator(); }
};

#endif
