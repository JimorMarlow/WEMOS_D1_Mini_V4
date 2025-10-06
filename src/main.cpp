#include <Arduino.h>

// WEMOS D1 Mini V4  встроенный синий светодиод, подключённый к пину D4 (GPIO2)
// #define LED_BUILTIN D4

//////////////////////////////////////////////////////////
// Platform dependent settings

#ifdef ESP8266
  bool INVERSE_BUILDING_LED = true;
#else
  // Код для других плат
  bool INVERSE_BUILDING_LED = false; 
#endif

//////////////////////////////////////////////////////////

#include "led.h"
LED blinkLED  (LED_BUILTIN, false, INVERSE_BUILDING_LED);

#include "morse.h"
MorseCode morse(&blinkLED);
uint32_t MORSE_INTERVAL = 5000;
GTimer timer_Morse(MS);               // создать миллисекундный таймер

// Запуск по интервалу
#include "GyverTimer.h"
GTimer timer_LED(MS);               // создать миллисекундный таймер
uint32_t BLINK_INTERVAL = 2000;
uint32_t BLINK_DURATION = 10;

void setup() {
    Serial.begin(115200);
    timer_LED.setTimeout(BLINK_INTERVAL);   // настроить интервал
    

    // morse.debug_trace("123");
    // morse.debug_trace("123 123");
    // morse.debug_trace("123.123,098");
    // morse.debug_trace("pirat123.123 dde");
    timer_Morse.setTimeout(MORSE_INTERVAL);

    Serial.println("start...");
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

    if (timer_Morse.isReady()) 
    {
        String text = "123.123,098";
        morse.debug_trace(text);
        morse.send(text);
        timer_Morse.setTimeout(MORSE_INTERVAL);
    }

}