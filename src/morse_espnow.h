#pragma once
// Передача данных с помощью ESPNOW для азбуки морзе
// Тестовый функционал для проеврки работы ESP8266 и ESP32 по одному протоколу
// Общая идея:
// <- Клиент передает серверу счетчик с интервалом 2-5секунд morse_message_t::type_t::kCount
// -> Сервер выводит счетчик на экран и начинает мигать встроенным светодиодом забукой морзе, и при этом отправляет клиенту команду мигать
// <- morse_message_t::type_t::kBlink
// Клиент ждет 2-5 секунд, после отправки температуры, если приходит команда мигать, то таймаут сбрасывается и запускается еще на цикл, 
// чтобы следующее измерение счетчика происходило либо по интервалу, если сервер не отвечает, либо через интервал после последнего ответа от сервера.
// Сервер ставит таймаут на интервала обновления счетчика, и, если новые данные не приходят, сбрасывает изображение на дисплее
// 
// Будет использоваться для проверки дальности работы по ESPNOW. Запускаем клиент и сервер, видим, что счетчик растет и принимается на сервере,
// отправляем клиента и следим, чтобы светодиод мигал. Замеряем точки, где трансляция нарушается.

#include "Arduino.h"
//#include "espnow/esp_manager.h"
#include "etl_espnow.h"
#include "etl_led.h"

// Мои модули для отладки
namespace esp_board {
    const etl::espnow::endpoint_t WEMOS_D1_Mini_v4_s001 {"F4:CF:A2:78:DF:F9"};    // Первая плата без крышки рассеивателя
    const etl::espnow::endpoint_t WEMOS_D1_Mini_v3_s002 {"EC:FA:BC:D5:A2:50"};    // Вторая с крышкой
    const etl::espnow::endpoint_t ESP32_C3_ProMini_s003 {"0C:4E:A0:68:5A:E4"};    // ESP32 C3 Pro Mini с гнездом 
}// namespace esp_board

// Структура для сообщений
struct morse_message_t {
    enum type_t : uint8_t {
        kEmpty = 0,
        kBlink,
        kCount
    };
    uint32_t timestamp = 0 ;
    type_t id = type_t::kEmpty;
    uint32_t value = 0;
};

class morse_relay_mgr : public etl::espnow::manager<morse_message_t>
{
private:
    etl::led* _led = nullptr;
public: 
    morse_relay_mgr(bool server){}
    void send_blink(uint32_t duration) {
        morse_message_t msg;
        msg.id = morse_message_t::type_t::kBlink;
        msg.value = duration;
    }

protected:
    virtual void on_data_recieve(const etl::espnow::endpoint_t& from, const uint8_t *incomingData, int len) override {
        // Базовая реализация - можно переопределить
        manager::on_data_recieve(from, incomingData, len);
        if(auto msg = get_last_received_message(); msg)
        {
            if(msg && msg->id == morse_message_t::type_t::kBlink && msg->value > 0)
            {
                // do blink in reciever
                if(_led) _led->blink(msg->value);       
            }
        }
    }
};
//espnow::manager<morse_message_t> morse_relay; // Передатчик данных по ESPNOW