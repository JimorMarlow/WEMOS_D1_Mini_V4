// Morse code для тренировки работы с таймерами и LED
// Передает заданные цифры морганием встроенного светодиода
#pragma once

#include "led.h"
#include "GyverTimer.h"

class MorseCode
{
    enum interval_t : int {
        dot     = 1,    // Точка	1 единица времени
        dash    = 3,    // Тире	    3 единицы времени
        pulse   = 1,    // Интервал между точкой/тире	1 единица времени
        symbol  = 3,    // Интервал между символами	3 единицы времени
        word    = 7     // Интервал между словами	7 единиц времени
    };

    enum code_t : char {
        DOT = '.',
        DASH = '-',
        PAUSE = ' ',
        WDBR = '|'
    };

public: 
    MorseCode(LED* led, uint32_t dit_duration_ms = 100);   // светодиод для моргания и стандартная длительность точки
    virtual ~MorseCode() = default;

    struct table_t
    {
        char    symbol;
        String  value;
    };

    void tick();    
    void send(const String& text);
    void reset();

    void debug_trace(const String& value);  

protected:
    String message_to_code(const String& text);
    String get_code(char ch);

protected:
    uint32_t dit_duration_ = 100;       // длительность единицы времени

    bool     transmitting_  = false;     // идет процесс передачи
    bool     is_completed_  = false;     // завершено
    String   message_       = "";        // Сообщение для передачи
    int      symbol_pos_    = -1;        // Указатель на текущий символ
    String   dit_code_      = "";        // последовательность точек-тире для текущего символа      
    int      dit_pos_       = -1;        // указатель на текущий элемент

    GTimer   timer_next_symbol_;        // миллисекундный таймер для передачи следующего символа
    GTimer   timer_next_dit_;           // миллисекундный таймер для передачи следующего элемента 

    table_t table_digits_ [10] {
        {'1', ".----"},
        {'2', "..---"},
        {'3', "...--"},
        {'4', "....-"},
        {'5', "....."},
        {'6', "-...."},
        {'7', "--..."},
        {'8', "---.."},
        {'9', "----."},
        {'0', "-----"}
    };
};
