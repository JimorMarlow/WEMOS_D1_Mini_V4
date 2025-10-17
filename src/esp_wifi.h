// Для включения нужной wi-fi библиотеки
#pragma once
#ifdef ESP8266
  #include <ESP8266WiFi.h>
#elif ESP32
  #include <WiFi.h>
#else
  #pragma message("ERROR: no Wi-Fi lib specified")
#endif

#include <etl_vector.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

struct mac_address_t
{
  etl::vector<uint8_t> mac{size_t(6), 0xFF};
  etl::vector<uint8_t> mac1 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  etl::vector<uint8_t> mac2 {broadcastAddress}; 
  etl::vector<uint8_t> mac3 = broadcastAddress; 
};