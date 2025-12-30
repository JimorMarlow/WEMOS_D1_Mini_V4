#include "test_littlefs.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>

namespace etl{
namespace test{
namespace littlefs{

bool start_littlefs()
{
    Serial.println("\n\n========================================");
    Serial.println("ESP32-C3 LittleFS Test");
    Serial.println("========================================\n");
    
    // Выводим информацию о системе
    Serial.printf("Chip: %s\n", ESP.getChipModel());
    Serial.printf("Cores: %d\n", ESP.getChipCores());
    Serial.printf("Flash size: %u bytes\n", ESP.getFlashChipSize());
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    
    // Инициализируем LittleFS
    if (!initLittleFS()) {
        Serial.println("\nCritical: Failed to initialize filesystem!");
        Serial.println("Possible causes:");
        Serial.println("1. Wrong partition table");
        Serial.println("2. Filesystem not formatted");
        Serial.println("3. Insufficient flash space");
        
        // Показываем информацию о разделах
        showPartitionInfo();
        return false;
    }
    
    // Создаем тестовый файл
    createTestFile();
    
    Serial.println("\n========================================");
    Serial.println("Setup complete!");
    Serial.println("========================================\n");

    return true;
}

void showPartitionInfo() {
    Serial.println("\nPartition information:");
    
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, 
                                                    ESP_PARTITION_SUBTYPE_ANY, 
                                                    NULL);
    while (it != NULL) {
        const esp_partition_t* p = esp_partition_get(it);
        Serial.printf("  %-12s: 0x%06X - 0x%06X (size: 0x%06X = %u bytes)\n",
                     p->label, p->address, p->address + p->size, 
                     p->size, p->size);
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
}

// Функция инициализации LittleFS с обработкой ошибок
bool initLittleFS() {
    Serial.println("Initializing LittleFS...");
    
    // Пробуем несколько способов
    bool success = false;
    
    // Способ 1: Без форматирования
    if (LittleFS.begin(false)) {
        Serial.println("✓ LittleFS mounted successfully");
        success = true;
    } else {
        Serial.println("✗ LittleFS mount failed, trying with formatting...");
        
        // Способ 2: С форматированием
        if (LittleFS.begin(true)) {
            Serial.println("✓ LittleFS mounted after formatting");
            success = true;
        } else {
            Serial.println("✗ All LittleFS mount attempts failed!");
            
            // Способ 3: Форматируем вручную
            Serial.println("Attempting manual format...");
            if (LittleFS.format()) {
                Serial.println("✓ Manual format successful");
                if (LittleFS.begin(false)) {
                    Serial.println("✓ LittleFS mounted after manual format");
                    success = true;
                }
            }
        }
    }
    
    if (success) {
        // Получаем информацию о файловой системе
        size_t total = LittleFS.totalBytes();
        size_t used = LittleFS.usedBytes();
        
        Serial.printf("LittleFS Info:\n");
        Serial.printf("  Total space: %u bytes\n", total);
        Serial.printf("  Used space:  %u bytes\n", used);
        Serial.printf("  Free space:  %u bytes\n", total - used);
        
        // Список файлов
        listFiles();
        
        return true;
    }
    
    return false;
}

// Функция для вывода списка файлов
void listFiles() {
    File root = LittleFS.open("/");
    if (!root) {
        Serial.println("Failed to open root directory");
        return;
    }
    
    if (!root.isDirectory()) {
        Serial.println("Root is not a directory");
        root.close();
        return;
    }
    
    Serial.println("Files in LittleFS:");
    
    File file = root.openNextFile();
    int count = 0;
    while (file) {
        Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
        file = root.openNextFile();
        count++;
    }
    
    if (count == 0) {
        Serial.println("  No files found");
    }
    
    root.close();
}

// Функция для создания тестового файла
void createTestFile() {
    File file = LittleFS.open("/test.txt", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to create test file");
        return;
    }
    
    file.println("Hello from ESP32-C3!");
    file.println("LittleFS is working!");
    file.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    file.close();
    
    Serial.println("Test file created: /test.txt");
    
    // Читаем обратно для проверки
    file = LittleFS.open("/test.txt", FILE_READ);
    if (file) {
        Serial.println("Test file contents:");
        while (file.available()) {
            Serial.write(file.read());
        }
        file.close();
    }
}

}// namespace litlefs
}// namespace test
}// namespace etl