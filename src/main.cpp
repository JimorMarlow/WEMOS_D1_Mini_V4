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
uint32_t BLINK_INTERVAL = 3000;
uint32_t BLINK_DURATION = 100;

#include "GyverTimer.h"
GTimer timer_LED(MS);               // создать миллисекундный таймер

void setup() {
    Serial.begin(115200);
    timer_LED.setTimeout(BLINK_INTERVAL);   // настроить интервал
}

void loop() 
{
    if(blinkLED.tick())
    {
      Serial.print(millis()); Serial.print(": "); Serial.println("off");
    }

    if (timer_LED.isReady()) 
    {
      blinkLED.blink(BLINK_DURATION); // blinkLED.toggle();      
      Serial.print(millis()); Serial.print(": "); Serial.println("blink");                    
      timer_LED.setTimeout(BLINK_INTERVAL);
    }
}