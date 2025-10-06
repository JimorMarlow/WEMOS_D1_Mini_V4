#include "morse.h"

MorseCode::MorseCode(LED* led, uint32_t dit_duration_ms /*= 100*/)  
: dit_duration_{dit_duration_ms}
{

}    

void MorseCode::send(const String& text)
{
    transmitting_ = true;     // идет процесс передачи
    is_completed_ = false;     // завершено
    message_ = text;
    symbol_pos_ = 0;
    timer_next_symbol_.setInterval(interval_t::pulse * dit_duration_);
}

void MorseCode::reset()
{
    transmitting_ = false;     // идет процесс передачи
    is_completed_ = false;     // завершено
    message_ = "";
    symbol_pos_  = -1;        // Указатель на текущий символ
    dit_code_  = "";        // последовательность точек-тире для текущего символа      
    dit_pos_   = -1;        // указатель на текущий элемент
}

String MorseCode::get_code(char ch)
{
    for(auto item : table_digits_)
    {
        if(item.symbol == ch)
        {
            return item.value;
        }
    }

    return String("");
}

String MorseCode::message_to_code(const String& text)
{
    String codes;
    for(char ch : text)
    {
        if(auto code = get_code(ch); !code.isEmpty())
        {
            codes += code;        // последовательность точек-тире для текущего символа      
            codes += code_t::PAUSE;        
        }
        else
        {
            if(!codes.isEmpty() && codes[codes.length()-1] == code_t::PAUSE) {
                codes[codes.length()-1] = code_t::WDBR;
            }
            else {
                codes += code_t::WDBR;
            }
        }        
    }

    return codes;    
}

void MorseCode::tick()
{
 
}

void MorseCode::debug_trace(const String& value)
{
    Serial.print(value);
    Serial.print(": ");
    Serial.println(message_to_code(value));
}