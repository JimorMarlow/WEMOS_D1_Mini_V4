#pragma once 
// отладка работы с littlefs

namespace etl{
namespace test{
namespace littlefs{

    bool start_littlefs();       // Инициализация littlefs и вывод доступной информациии

    // Функция инициализации LittleFS с обработкой ошибок
    bool initLittleFS();        // Функция инициализации LittleFS с обработкой ошибок
    void showPartitionInfo();   // Показать информацию о разделах
    void createTestFile();      // Функция для создания тестового файла    
    void listFiles();           // Функция для вывода списка файлов

}// namespace litlefs
}// namespace test
}// namespace etl