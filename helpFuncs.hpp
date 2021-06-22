#ifndef _HELPFUNCS_HPP_
#define _HELPFUNCS_HPP_

#include <list>
#include <tuple>
#include "consts.hpp"

template <typename T, typename Alloc, template <typename, typename> class V>
V<T, Alloc> operator+(V<T, Alloc> first, V<T, Alloc> second) {
    V<T, Alloc> result;

    for (T& value : first)
        result.push_back(value);

    for (T& value : second)
        result.push_back(value);

    return result;
}

template <typename T, typename Alloc, template <typename, typename> class V>
void operator+=(V<T, Alloc> first, V<T, Alloc> second) {
    for (T& value : second)
        first.push_back(value);
}

#endif // _HELPFUNCS_HPP_
