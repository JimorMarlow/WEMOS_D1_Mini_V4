// LED - простой класс для мигания светодиодом
#pragma once

#include "GyverTimer.h"

class LED
{
public:
    LED(int pin, bool state, bool inverse = false);
    virtual ~LED() = default;

    virtual bool tick();        // out: true - изменилось состояние по внутреннему таймеру

    bool get_state();   // вернуть состояние из внутренней переменно
    bool set_state(bool state); // Установить новое состояние и записать в порт, если нужно, вернуть новое состояние

    bool read_state();  // считать состояние из порта
    void write_state(bool state); // записать состояние в порт

    virtual void on();
    virtual void off();
    virtual void blink(uint32_t delay_ms);
    virtual void toggle();
    virtual void reset();   // сбросить таймеры моргания, если были
   
protected:
    int     pin_    = -1;
    bool    state_  = false;
    bool    inverse_ = false;   // инвертировать состояние для некоторых пинов (н-р, встроенный led для esp8266)

    GTimer timer_Blink;                 // создать миллисекундный таймер для управления морганием
};