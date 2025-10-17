#pragma once
#include <Arduino.h>

//////////////////////////////////////////////////////////
// Platform dependent settings
// Если добавляем в platformio.ini новую среду [env] необходимо для этой платы добавить define в 
// параметрах компиляции для этой секции и по ней разделить несопадающие конфигурации
// BOARD_xxx 

#ifdef BOARD_WEMOS_D1_MINI  // #ifdef ESP8266
   #pragma message("Компилируется под WEMOS D1 MINI")
  bool INVERSE_BUILTING_LED = true;
  const int LED_MORSE = LED_BUILTIN;
  // WEMOS D1 Mini V4  встроенный синий светодиод, подключённый к пину D4 (GPIO2)
  // #define LED_BUILTIN D4
  const int SERIAL_INIT_DELAY = 0;
#elif BOARD_ESP32_C3_SUPER_MINI // #elif ESP32
  // Код только для ESP32-C3
  #pragma message("Компилируется под ESP32-C3 super mini")
  bool INVERSE_BUILTING_LED = true;
  const int LED_MORSE = 8; //LED_BUILTIN; НЕЛЬЗЯ встроенный, он не совпадает с нашей ESP32 C3 PRO MINI и уводит плату в панику при записи в 30 GPIO, нужно напрямую указать 8
  const int SERIAL_INIT_DELAY = 1000; // для ESP32 C3 supermini нуждо сделать задержку, чтобы выводилась отладочная информация
#else
  // Код для других плат
  #pragma message("Компилируется под неизвестную плату. Добавьте BOARD_xxx в platformio.ini для нужной секции [env:xxx]")
  bool INVERSE_BUILTING_LED = false; 
  const int LED_MORSE = LED_BUILTIN;
#endif