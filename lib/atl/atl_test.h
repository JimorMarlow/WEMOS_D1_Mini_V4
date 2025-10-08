// проверка atl - Arduino Template Library,
// STL-like облегченная библиотека для простых элементов из стандартной библиотеки
#pragma once 
#include "Arduino.h"

namespace atl
{
    bool test_all(Stream& trace);
    String test_optional();
}