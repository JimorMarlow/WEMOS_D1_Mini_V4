#pragma once 
// Локальный веб-сервер для настроек и проверки работы с littleFS
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>  // Для ESP32
// #include <ESP8266mDNS.h>  // Для ESP8266

class web_ui 
{
    AsyncWebServer server;

public:
    web_ui() : server(80) {}
    virtual ~web_ui() = default;

    bool setup();
};