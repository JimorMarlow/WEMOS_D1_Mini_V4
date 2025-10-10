// проверка elt - Embedded Lightweight Template library,
// STL-like облегченная библиотека для простых элементов из стандартной библиотеки
#pragma once 
#include "Arduino.h"

namespace etl
{
    bool test_all(Stream& trace);
    String test_optional(Stream& trace);
}