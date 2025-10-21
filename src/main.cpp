#include <Arduino.h>

// Вся конфигурация и платформозависимые настройки в pinout.h
#include "pinout.h"
//////////////////////////////////////////////////////////
#include "etl_memory.h"
#include <GTimer.h>

#include "led.h"
LED blinkLED  (LED_MORSE, false, INVERSE_BUILTING_LED);

#include "morse_espnow.h"
morse_relay_mgr morse_relay(true); // Передатчик данных по ESPNOW

#include "morse.h"
const uint32_t MORSE_DIT = 50;  // длительность единичного интервала (dit), для новичков 50-150 мс.
etl::unique_ptr<MorseCode> morse;// = etl::make_unique<MorseCode>(&blinkLED, MORSE_DIT); // светодиод и длительность единичного интервала (dit)

#ifdef MORSE_CLIENT
etl::unique_ptr<MorseCode> morse_client;// = etl::make_unique<MorseCode>(&blinkLED, MORSE_DIT); 
#elif MORSE_SERVER
etl::unique_ptr<MorseCode> morse_server;// = etl::make_unique<MorseCode>(&blinkLED, MORSE_DIT); 
#endif
const uint32_t MORSE_INTERVAL = 5000;
GTimer<millis> timer_Morse;               // создать миллисекундный таймер

// Запуск по интервалу
GTimer<millis> timer_LED;
uint32_t BLINK_INTERVAL = 2000;
uint32_t BLINK_DURATION = 10;

/////////////////////////////////////////
// atl - отладка функционала
#include "etl_test.h"
/////////////////////////////////////////
//#include "espnow/esp_manager.h"

void setup() {
    Serial.begin(115200);
    if(SERIAL_INIT_DELAY > 0) delay(SERIAL_INIT_DELAY);  // для ESP32 C3 supermini нуждо сделать задержку, чтобы выводилась отладочная информация
    
    blinkLED.off();
    blinkLED.blink(BLINK_DURATION);
    timer_LED.start(BLINK_INTERVAL, GTMode::Timeout);   // настроить интервал
  
    // morse.debug_trace("123");
    // morse.debug_trace("123 123");
    // morse.debug_trace("123.123,098");
    // morse.debug_trace("pirat123.123 dde");
   if(morse)
     timer_Morse.start(MORSE_INTERVAL, GTMode::Timeout);

    Serial.println("start...");

    /////////////////////////////////////////
    // atl - отладка функционала
    etl::test_all(Serial);
    /////////////////////////////////////////

    Serial.println("-----------WIFI----------");
    Serial.print("SSID: ");  Serial.println(WIFI_SSID);
    Serial.print("PASS: ");  Serial.println(WIFI_PASS);
    Serial.print("MODE: ");  Serial.println(MORSE_MODE);
    
    // Подключение к Wi-Fi не требуется для получения MAC-адреса.
    // WiFi.mode(WIFI_STA); // Устанавливаем режим работы (в данном случае, как станция)
    Serial.print("MAC : ");  Serial.println(espnow::board::get_mac_address());
    Serial.println("-------------------------");
}

void loop() 
{
    // blinkLED.tick(); // если не используется morse нужно вызывать тут для обновления внутреннего таймера
    // if(blinkLED.tick())
    // {
    //   Serial.print(millis()); Serial.print(": "); Serial.println("off");
    // }

    // if (timer_LED.isReady()) 
    // {
    //   blinkLED.blink(BLINK_DURATION); // blinkLED.toggle();      
    //   Serial.print(millis()); Serial.print(": "); Serial.println("blink");                    
    //   timer_LED.start(BLINK_INTERVAL, GTMode::Timeout)
    // }

    ///////////////////////////////////////////////////
    // Проверка моргания светодиодом по таймеру
    if(morse) {
      morse->tick();
      if (timer_Morse.tick()) 
      {
          // String text = "ntcn 123.123,098";
          String text = String(millis());
          uint32_t transmittion_duration_ms = morse->send(text);
          timer_Morse.start(MORSE_INTERVAL + transmittion_duration_ms, GTMode::Timeout);
      }
    }
    else {
      blinkLED.tick(); // если не используется morse нужно вызывать тут для обновления внутреннего таймера
    }

    ///////////////////////////////////////////////////
    // Проверка коммуникации esp-now
#ifdef MORSE_CLIENT
    if(morse_client) morse_client->tick();
#elif MORSE_SERVER
    if(morse_server) morse_server->tick();
#endif
}