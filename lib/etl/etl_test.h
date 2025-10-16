// проверка elt - Embedded Lightweight Template library,
// STL-like облегченная библиотека для простых элементов из стандартной библиотеки
#pragma once 
#include "Arduino.h"

namespace etl
{
    bool test_all(Stream& trace);
    void test_result(Stream& trace, const String& title, const String& result);

    // тесты модулей
    String test_empty();
    String test_optional();
    String test_unique();
    String test_queue();
    String test_vector();

    // проверка алгоритмов 
    void test_average_filter(Stream& trace);
}