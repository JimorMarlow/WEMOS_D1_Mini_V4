#pragma once

// Основные инструменты для ETL

#include "Arduino.h"

#define UNREFERENCED_PARAMETER(P) ((void)(P))

namespace etl
{

// begin/end для массивов
template<typename T, size_t N>
constexpr T* begin(T (&arr)[N]) {
    return arr;
}

template<typename T, size_t N>
constexpr T* end(T (&arr)[N]) {
    return arr + N;
}

// begin/end для контейнеров
template<typename Container>
constexpr auto begin(Container& c) -> decltype(c.begin()) {
    return c.begin();
}

template<typename Container>
constexpr auto end(Container& c) -> decltype(c.end()) {
    return c.end();
}

// Простой аналог std::move
template<typename T>
struct remove_reference {
    typedef T type;
};

template<typename T>
struct remove_reference<T&> {
    typedef T type;
};

template<typename T>
struct remove_reference<T&&> {
    typedef T type;
};

template<typename T>
constexpr typename remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(arg);
}

// Минимальная реализация для экономии памяти
template<typename T>
T&& arduino_move(T& arg) {
    return static_cast<T&&>(arg);
}

template<typename T>
T&& arduino_move(T&& arg) {
    return static_cast<T&&>(arg);
}

template <typename T>
void swap(T& lhs, T&rhs)
{
    T tm = move(lhs); 
    lhs = move(rhs);
    rhs = move(tm);
}

// Базовая версия clamp
template<typename T>
constexpr const T& clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (value > high) ? high : value;
}

// Версия с компаратором
template<typename T, typename Compare>
constexpr const T& clamp(const T& value, const T& low, const T& high, Compare comp) {
    return comp(value, low) ? low : comp(high, value) ? high : value;
}

}// namespace etl