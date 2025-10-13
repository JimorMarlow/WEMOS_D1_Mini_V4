#include <Arduino.h>


//////////////////////////////////////////////////////////
// Platform dependent settings

#ifdef ESP8266
  bool INVERSE_BUILDING_LED = true;
  // WEMOS D1 Mini V4  встроенный синий светодиод, подключённый к пину D4 (GPIO2)
// #define LED_BUILTIN D4

#else
  // Код для других плат
  bool INVERSE_BUILDING_LED = false; 
  // ESP32-C3-mini  встроенный синий светодиод, подключённый к пину 8 (GPIO8)
  // #define LED_BUILTIN 8
#endif

//////////////////////////////////////////////////////////
#include "etl_memory.h"

#include "led.h"
LED blinkLED  (LED_BUILTIN, false, INVERSE_BUILDING_LED);

#include "morse.h"
const uint32_t MORSE_DIT = 50;  // длительность единичного интервала (dit), для новичков 50-150 мс.
etl::unique_ptr<MorseCode> morse = etl::make_unique<MorseCode>(&blinkLED, MORSE_DIT); // светодиод и длительность единичного интервала (dit)
const uint32_t MORSE_INTERVAL = 10000;
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
    etl::test_all(Serial);
    /////////////////////////////////////////
}

void loop() 
{
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