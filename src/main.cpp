#include <Arduino.h>

// Вся конфигурация и платформозависимые настройки в pinout.h
#include "pinout.h"
//////////////////////////////////////////////////////////
#include "etl/etl_memory.h"
#include <GTimer.h>

#include "etl/etl_led.h"
etl::shared_ptr<etl::led> blinkLED = etl::make_shared<etl::led>(LED_MORSE, false, INVERSE_BUILTING_LED);
etl::shared_ptr<etl::led> fadeLED;// = etl::make_shared<etl::led>(LED_FADE, false);

#include "morse_espnow.h"
morse_relay_mgr morse_relay(true); // Передатчик данных по ESPNOW

#include "morse.h"
const uint32_t MORSE_DIT = 50;  // длительность единичного интервала (dit), для новичков 50-150 мс.
etl::unique_ptr<MorseCode> morse = etl::make_unique<MorseCode>(blinkLED, MORSE_DIT); // светодиод и длительность единичного интервала (dit)

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

uint32_t FADE_INTERVAL = 3000;
uint32_t FADE_PAUSE = 3000;
bool fade_direction = true;
etl::unique_ptr<GTimer<millis>> time_fade_pause;

/////////////////////////////////////////
// atl - отладка функционала
#include "etl/etl_test.h"
/////////////////////////////////////////
#include "etl/etl_espwifi.h"

void setup() {
    Serial.begin(115200);
    if(SERIAL_INIT_DELAY > 0) delay(SERIAL_INIT_DELAY);  // для ESP32 C3 supermini нуждо сделать задержку, чтобы выводилась отладочная информация
    
    blinkLED->off();
    blinkLED->blink(BLINK_DURATION);
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
    etl::unittest::test_all(Serial);
    /////////////////////////////////////////

    Serial.println("-----------WIFI----------");
    Serial.print("SSID: ");  Serial.println(WIFI_SSID);
    Serial.print("PASS: ");  Serial.println(WIFI_PASS);
    Serial.print("MODE: ");  Serial.println(MORSE_MODE);
    Serial.print("MAC : ");  Serial.println(etl::espnow::board::get_mac_address());
    Serial.println("-------------------------");

    if(fadeLED) {
      Serial.println("fade started...");
      fadeLED->init_pwm(FADE_CHANNEL, 30000, 10); // Чтобы не было слышно пищания на низкой частоте - сделать 30КГц и максимально возможное разрешение 10 бит для плавности
      if(fade_direction) fadeLED->fade_in(FADE_INTERVAL); else fadeLED->fade_out(FADE_INTERVAL);
    }
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
      blinkLED->tick(); // если не используется morse нужно вызывать тут для обновления внутреннего таймера
    }

    // плавное включение и выключение LED, в выключенном положении пауза, чтобы оценить правильность затемнения без мерцания
    // if(time_fade_pause && time_fade_pause->tick())
    // {
    //   // start new cycle
    //   time_fade_pause.reset();
    //   fade_direction = true;
    //   if(fadeLED) fadeLED->fade_in(FADE_INTERVAL); 
    // }
    // else
    // {
    //   if(fadeLED && fadeLED->tick()) // 
    //   {
    //     fade_direction = !fade_direction;
    //     if(fade_direction)
    //     {
    //       // На новом цикле делаем паузу в выключенном состоянии, чтобы посмотреть, не мигает ли лента
    //       time_fade_pause = etl::make_unique<GTimer<millis>>(FADE_PAUSE, true, GTMode::Interval);
    //     }
    //     else
    //     {
    //   //  Serial.printf("main: fade %s\n", fade_direction ? "in" : "out");
    //       fadeLED->fade_out(FADE_INTERVAL);
    //     }
    //   }
    // }

    ///////////////////////////////////////////////////
    // Проверка коммуникации esp-now
#ifdef MORSE_CLIENT
    if(morse_client) morse_client->tick();
#elif MORSE_SERVER
    if(morse_server) morse_server->tick();
#endif
}