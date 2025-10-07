#include "morse.h"

MorseCode::MorseCode(LED* led, uint32_t dit_duration_ms /*= 100*/)  
: led_(led)
, dit_duration_{dit_duration_ms}
{

}    

uint32_t MorseCode::send(const String& text)
{
    reset();
    uint32_t duration_ms = 0;
    if(dit_code_ = message_to_code(text); !dit_code_.isEmpty())
    {
        duration_ms = get_dit_code_duration(dit_code_);
        debug_trace(text);        
        transmitting_ = true;     // идет процесс передачи
        is_completed_ = false;     // завершено
        dit_pos_ = 0;
        timer_next_.setInterval(interval_t::pulse * dit_duration_);
    }
    
    return duration_ms;
}

void MorseCode::reset()
{
    if(led_) led_->off();
    transmitting_ = false;     // идет процесс передачи
    is_completed_ = false;     // завершено
    dit_code_  = "";        // последовательность точек-тире для текущего символа      
    dit_pos_   = -1;        // указатель на текущий элемент
}

String MorseCode::get_char_code(char ch)
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
    static String separators = " ,.|!?-/\\";
    String codes;
    for(char ch : text)
    {
        if(auto code = get_char_code(ch); !code.isEmpty())
        {
            codes += code;        // последовательность точек-тире для текущего символа      
            codes += code_t::PAUSE;        
        }
        else
        {
            for(auto sep : separators)
            {
                if(ch == sep)
                {
                    // Добавить раздилитель
                    if(!codes.isEmpty() && codes[codes.length()-1] == code_t::PAUSE) {
                        codes[codes.length()-1] = code_t::WDBR; 
                    }
                    else {
                        codes += code_t::WDBR;
                    }
                }
                else
                {
                    // Игнорируем неизвестные символы, если нужно, потом добавить в справочник нужные коды
                    // :TODO: skiping symbols
                }
            }            
        }        
    }

    return codes;    
}

void MorseCode::tick()
{
    if(led_) led_->tick();
    if(timer_next_.isReady())
    {
        if(dit_pos_ < static_cast<int>(dit_code_.length()))
        {
            // Передать текущий символ
            switch(dit_code_[dit_pos_])
            {
            case code_t::DOT:
                if(led_) led_->blink(interval_t::dot * dit_duration_);
                timer_next_.setTimeout((interval_t::dot + interval_t::pulse) * dit_duration_);
            //    debug_trace_dit(code_t::DOT, interval_t::dot, interval_t::pulse);
                break;        
            case code_t::DASH:
                if(led_) led_->blink(interval_t::dash * dit_duration_);
                timer_next_.setTimeout((interval_t::dash + interval_t::pulse) * dit_duration_);
            //    debug_trace_dit(code_t::DASH, interval_t::dash, interval_t::pulse);
                break;        
            case code_t::PAUSE:
                if(led_) led_->off();
                timer_next_.setTimeout(interval_t::symbol * dit_duration_);
            //    debug_trace_dit(code_t::PAUSE, 0, interval_t::symbol);
                break;        
            case code_t::WDBR:
                if(led_) led_->off();
                timer_next_.setTimeout(interval_t::word * dit_duration_);
            //    debug_trace_dit(code_t::WDBR, 0, interval_t::word);
                break;        
            default:
                Serial.println("ERROR: Неизвестный символ!");
                break;
            }

            // Переместить указатель к следующему
            dit_pos_++;
        }
        else
        {
            // стоп, все данные переданы
            transmitting_ = false;    // идет процесс передачи
            is_completed_ = true;     // завершено
        }
    }
}

uint32_t MorseCode::get_dit_code_duration(const String& dit_code)
{
    int dit_count = 0;

    for(auto item : dit_code)
    {
        // Передать текущий символ
        switch(item)
        {
        case code_t::DOT:
            dit_count += (interval_t::dot + interval_t::pulse);
            break;        
        case code_t::DASH:
            dit_count += (interval_t::dash + interval_t::pulse);
            break;        
        case code_t::PAUSE:
            dit_count += interval_t::symbol;
            break;        
        case code_t::WDBR:
            dit_count += interval_t::word;
            break;        
        default:
            break;
        }
    }

    return dit_count * dit_duration_;
}

void MorseCode::debug_trace(const String& value)
{
    Serial.print(millis()); Serial.print(" ms| send: ");
    Serial.print(value);
    Serial.print(" : ");
    Serial.println(message_to_code(value));
}

void MorseCode::debug_trace_dit(code_t dit, int on_count, int off_count)
{
    Serial.print(millis());     Serial.print(" ms| send: '");
    Serial.print(dit);          Serial.print("' : ");
    for(int i = 0; i < on_count; ++i) Serial.print("*");
    for(int i = 0; i < off_count; ++i) Serial.print("_");
    Serial.println();
}