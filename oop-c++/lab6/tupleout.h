#ifndef TUPLEOUT_H
#define TUPLEOUT_H

#include <ostream>
#include <tuple>

/*
Рекурсия идёт по индексу элемента: printTuple<0> печатает нулевой элемент и
вызывает printTuple<1>, и так до Index == sizeof...(Args), где рекурсия
обрывается. if constexpr отбрасывает мёртвую ветку на этапе компиляции,
поэтому обращения к std::get<Index> за границами кортежа не возникает.
*/

template <std::size_t Index = 0, typename Ch, typename Tr, typename... Args>
std::basic_ostream<Ch, Tr>& printTuple(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& value) {
    if constexpr (Index < sizeof...(Args)) {
        if constexpr (Index > 0) {
            os << ", ";
        }
        os << std::get<Index>(value);
        return printTuple<Index + 1>(os, value);
    } else {
        return os;
    }
}

template <typename Ch, typename Tr, typename... Args>
std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& value) {
    return printTuple(os, value);
}

#endif
