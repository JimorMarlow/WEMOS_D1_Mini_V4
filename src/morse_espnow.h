// Передача данных с помощью ESPNOW для азбуки морзе
#include "Arduino.h"
#include "espnow/esp_manager.h"
#include "led.h"

// Структура для сообщений
struct morse_message_t {
    enum type_t : uint8_t {
        kEmpty = 0,
        kBlink
    };
    uint32_t timestamp = 0 ;
    type_t id = type_t::kEmpty;
    uint32_t blink = 0;
};

class morse_relay_mgr : public espnow::manager<morse_message_t>
{
private:
    LED* _led = nullptr;
public: 
    morse_relay_mgr(bool server){}
    void send_blink(uint32_t duration) {
        morse_message_t msg;
        msg.id = morse_message_t::type_t::kBlink;
        msg.blink = duration;
    }

protected:
    virtual void on_data_recieve(const espnow::endpoint_t& from, const uint8_t *incomingData, int len) override {
        // Базовая реализация - можно переопределить
        manager::on_data_recieve(from, incomingData, len);
        if(auto msg = get_last_received_message(); msg)
        {
            if(msg && msg->id == morse_message_t::type_t::kBlink && msg->blink > 0)
            {
                // do blink in reciever
                if(_led) _led->blink(msg->blink);       
            }
        }
    }
};
//espnow::manager<morse_message_t> morse_relay; // Передатчик данных по ESPNOW