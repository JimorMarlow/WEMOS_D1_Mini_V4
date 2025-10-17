#include <Arduino.h>


//////////////////////////////////////////////////////////
// Platform dependent settings

#ifdef ESP8266
   #pragma message("Компилируется под WEMOS D1 MINI")
  bool INVERSE_BUILDING_LED = true;
  const int LED_MORSE = LED_BUILTIN;
  // WEMOS D1 Mini V4  встроенный синий светодиод, подключённый к пину D4 (GPIO2)
// #define LED_BUILTIN D4
#elif ESP32
  // Код только для ESP32-C3
  #pragma message("Компилируется под ESP32-C3 mini")
  bool INVERSE_BUILDING_LED = true;
  const int LED_MORSE = 8; //LED_BUILTIN; НЕЛЬЗЯ встроенный, он не совпадает с нашей ESP32 C3 PRO MINI и уводит плату в панику при записи в 30 GPIO, нужно напрямую указать 8
#else
  // Код для других плат
  bool INVERSE_BUILDING_LED = false; 
  const int LED_MORSE = LED_BUILTIN;
#endif

//////////////////////////////////////////////////////////
#include "etl_memory.h"

#include "led.h"
LED blinkLED  (LED_MORSE, false, INVERSE_BUILDING_LED);

#include "morse.h"
const uint32_t MORSE_DIT = 50;  // длительность единичного интервала (dit), для новичков 50-150 мс.
etl::unique_ptr<MorseCode> morse = etl::make_unique<MorseCode>(&blinkLED, MORSE_DIT); // светодиод и длительность единичного интервала (dit)
const uint32_t MORSE_INTERVAL = 5000;
GTimer timer_Morse(MS);               // создать миллисекундный таймер

// Запуск по интервалу
#include "GyverTimer.h"
GTimer timer_LED(MS);               // создать миллисекундный таймер
uint32_t BLINK_INTERVAL = 2000;
uint32_t BLINK_DURATION = 10;

/////////////////////////////////////////
// atl - отладка функционала
#include "etl_test.h"
/////////////////////////////////////////

void setup() {
    Serial.begin(115200);
    #ifdef ESP32
    delay(1000);  // для ESP32 C3 supermini нуждо сделать задержку, чтобы выводилась отладочная информация
    #endif
    
    blinkLED.off();
    blinkLED.blink(BLINK_DURATION);
    timer_LED.setTimeout(BLINK_INTERVAL);   // настроить интервал
  
    // morse.debug_trace("123");
    // morse.debug_trace("123 123");
    // morse.debug_trace("123.123,098");
    // morse.debug_trace("pirat123.123 dde");
   if(morse)
     timer_Morse.setTimeout(MORSE_INTERVAL);

    Serial.println("start...");

    /////////////////////////////////////////
    // atl - отладка функционала
  //  etl::test_all(Serial);
    /////////////////////////////////////////
}

void loop() 
{
    // blinkLED.tick();
    // if(blinkLED.tick())
    // {
    //   Serial.print(millis()); Serial.print(": "); Serial.println("off");
    // }

    // if (timer_LED.isReady()) 
    // {
    //   blinkLED.blink(BLINK_DURATION); // blinkLED.toggle();      
    //   Serial.print(millis()); Serial.print(": "); Serial.println("blink");                    
    //   timer_LED.setTimeout(BLINK_INTERVAL);
    // }

    if(morse) 
    {
      morse->tick();
      if (timer_Morse.isReady()) 
      {
          // String text = "ntcn 123.123,098";
          String text = String(millis());
          uint32_t transmittion_duration_ms = morse->send(text);
          timer_Morse.setTimeout(MORSE_INTERVAL + transmittion_duration_ms);
      }
    }
}