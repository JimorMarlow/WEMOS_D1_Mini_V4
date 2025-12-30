#include "web_ui.h"

bool web_ui::setup()
{
// Инициализация файловой системы
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        Serial.println("Formatting...");
        if (LittleFS.format()) {
            Serial.println("Formatted successfully");
            if (!LittleFS.begin()) {
                Serial.println("Still failed after format!");
                return false;
            }
        } else {
            Serial.println("Format failed!");
            return false;
        }
    }
    
    Serial.println("LittleFS mounted successfully");
    
    // Список файлов (для отладки)
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
        Serial.printf("File: %s (%d bytes)\n", file.name(), file.size());
        file = root.openNextFile();
    }
    root.close();
    
    // Настройка WiFi
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("ESP-Config", "12345678");
    WiFi.begin("YourSSID", "YourPassword");
    
    // Веб-сервер
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        if (LittleFS.exists("/index.html")) {
            request->send(LittleFS, "/index.html", "text/html");
        } else {
            request->send(200, "text/plain", "Index.html not found in LittleFS");
        }
    });
    
    // Статические файлы
    server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/css/style.css", "text/css");
    });
    
    server.on("/js/app.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/js/app.js", "application/javascript");
    });
    
    // API endpoints
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{";
        json += "\"wifi\":\"" + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "\",";
        json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        json += "\"uptime\":\"" + String(millis() / 1000) + "\"";
        json += "}";
        request->send(200, "application/json", json);
    });
    
    server.on("/api/light", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("state")) {
            String state = request->getParam("state")->value();
            // Здесь управление светом
            request->send(200, "text/plain", state);
        } else {
            request->send(400, "text/plain", "Missing state parameter");
        }
    });
    
    server.on("/api/brightness", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("value")) {
            int brightness = request->getParam("value")->value().toInt();
            // Здесь установка яркости
            request->send(200, "text/plain", "Brightness: " + String(brightness));
        }
    });
    
    // Обслуживание всех статических файлов
    server.serveStatic("/", LittleFS, "/");
    
    // Запуск сервера
    server.begin();
    Serial.println("HTTP server started");
    
    // mDNS для доступа по имени
    if (MDNS.begin("esp32")) {  // esp8266 для ESP8266
        MDNS.addService("http", "tcp", 80);
        Serial.println("mDNS responder started: http://esp32.local");

        return true;
    }

    return false;
}