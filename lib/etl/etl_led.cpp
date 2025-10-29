#include "etl_led.h"

namespace etl {

LED::LED(int pin, bool state, bool inverse /*= false*/)
 : pin_  {pin}
 , state_{state}
 , inverse_{inverse}
{
    pinMode(pin_, OUTPUT); // Инициализация пина как выход
    bool cur_state = read_state();
    if(cur_state != state_)
        write_state(state_);
}

bool LED::get_state()   // вернуть состояние из внутренней переменно
{
   return state_;
}
    
bool LED::set_state(bool state) // Установить новое состояние и записать в порт, если нужно, вернуть новое состояние
{
    if(state_ != state)
    {
        state_ = state;
        if(read_state() != state_)
        {
            write_state(state_);
        }
    }

    return state_;
}
    
bool LED::read_state()  // считать состояние из порта
{
    auto value = digitalRead(pin_);
    return inverse_ ? value == LOW : value == HIGH;
}

void LED::write_state(bool state) // записать состояние в порт
{
    auto new_state = (inverse_ ? !state : state) ? HIGH : LOW;  
    digitalWrite(pin_, new_state);
}

void LED::on()
{
    set_state(true);
}
    
void LED::off()
{
    set_state(false);
}

void LED::toggle()
{
    set_state(!get_state());
}

void LED::blink(uint32_t delay_ms)
{
    toggle();
    timer_Blink.start(delay_ms, GTMode::Timeout);
}

void LED::reset()   // сбросить таймеры моргания, если были
{
    timer_Blink.stop();
}

bool LED::tick()
{
    if(timer_Blink.tick())
    {
        toggle();
        return true;
    }
    return false;
}

} //namespace etl