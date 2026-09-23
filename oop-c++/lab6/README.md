# Лабораторная 6 — Парсер CSV на шаблонах

**Период:** 01.09.2025 — 31.12.2025

**Язык:** C++ · **Сборка:** CMake

Шаблонный парсер CSV, который отдаёт строки файла сразу нужными типами:
`CSVParser<int, std::string, double>` возвращает `std::tuple<int, std::string,
double>`. Файл читается лениво — по строке за инкремент итератора, всё
содержимое в память не загружается.

## Использование

```cpp
std::ifstream file("data.csv");
CSVParser<int, std::string, double> parser(file, 1);  // 1 — пропустить заголовок

for (const std::tuple<int, std::string, double>& row : parser) {
    std::cout << row << std::endl;
}
```

```bash
./csvparser data.csv 1
```

## Печать кортежа

Оператор вывода для `std::tuple` написан рекурсией по индексу элемента:
`printTuple<0>` печатает нулевой элемент и вызывает `printTuple<1>`, и так до
`Index == sizeof...(Args)`, где рекурсия обрывается.

```cpp
template <std::size_t Index = 0, typename Ch, typename Tr, typename... Args>
std::basic_ostream<Ch, Tr>& printTuple(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& value);
```

`if constexpr` отбрасывает мёртвую ветку ещё на этапе компиляции, поэтому
обращения к `std::get<Index>` за границами кортежа не возникает и отдельная
терминальная перегрузка через `enable_if` не нужна.

## Настройка и экранирование

| Параметр | По умолчанию | Значение |
| --- | --- | --- |
| `linesToSkip` | `0` | сколько строк пропустить в начале файла |
| `column` | `,` | разделитель колонок |
| `row` | `\n` | разделитель строк |
| `escapeSymbol` | `"` | символ экранирования |

Ячейка, начинающаяся с символа экранирования, читается до парной закрывающей
кавычки — разделитель внутри неё остаётся обычным символом. Удвоенная кавычка
внутри экранированной ячейки означает одну кавычку в данных.

Строковые колонки берутся как есть — потоковое чтение оборвалось бы на первом
пробеле. Остальные типы читаются через `std::istringstream` и обязаны быть
прочитаны целиком: остаток в ячейке означает, что данные не подходят колонке.

## Ошибки разбора

`CSVError` наследует `std::runtime_error` и хранит номер строки и номер
колонки, обе нумерации с единицы.

| Ситуация | Сообщение |
| --- | --- |
| значение не подходит типу колонки | `line 3, column 3: 'notanumber' does not fit the column type` |
| не столько колонок, сколько ожидалось | `line 2, column 3: expected 3 columns, got 2` |
| кавычка не закрыта | `line 2, column 2: escaped cell is not closed` |
| текст после закрывающей кавычки | `line 2, column 2: unexpected text after the closing quote` |

## Файлы

```
main.cpp        разбор аргументов, демонстрация обхода файла
tupleout.h      рекурсивный operator<< для std::tuple
csvparser.h     CSVParser, его Iterator, разбор строки и CSVError
data.csv        пример данных с экранированием
```

Парсер шаблонный целиком, поэтому живёт в заголовках — отдельного `.cpp` у
него нет.

## Сборка и запуск

```bash
cmake -B build && cmake --build build
./build/csvparser data.csv 1
```
