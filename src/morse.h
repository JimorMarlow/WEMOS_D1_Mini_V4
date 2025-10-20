// Morse code для тренировки работы с таймерами и LED
// Передает заданные цифры морганием встроенного светодиода
#pragma once

#include "led.h"
#include "GTimer.h"
#include "etl_vector.h"

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
    MorseCode(LED* led, uint32_t dit_duration_ms = 50);   // светодиод для моргания и стандартная длительность точки
    virtual ~MorseCode() = default;

    struct table_t
    {
        char    symbol;
        String  value;
    };

    void tick();    
    uint32_t send(const String& text);  // return: tramsmitting duration in ms 
    void reset();

    void debug_trace(const String& value);  
    void debug_trace_dit(code_t dit, int on_count, int off_count);  

protected:
    etl::vector<char> message_to_code(const String& text);
    String get_char_code(char ch);
    uint32_t get_dit_code_duration(const etl::vector<char>& dit_code);

protected:
    LED*     led_           = nullptr;   // светодиод для передачи кода
    uint32_t dit_duration_  = 100;       // длительность единицы времени

    bool     transmitting_  = false;     // идет процесс передачи
    bool     is_completed_  = false;     // завершено
    etl::vector<char> dit_code_;         // последовательность точек-тире для текущей передачи
    int      dit_pos_       = -1;        // указатель на текущий элемент

    GTimer<millis>  timer_next_;        // миллисекундный таймер для передачи следующего символа
    
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
