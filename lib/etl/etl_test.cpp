#include "etl_test.h"
#include "etl_optional.h"
#include "etl_math.h"
#include "etl_memory.h"
#include "etl_queue.h"
#include "etl_vector.h"
#include "etl_array.h"
#include "etl_utility.h"
#include "etl_lookup.h"
#include "filter/moving_average.h"
#include "filter/exponential.h"
#include "filter/median.h"
#include "sensor/temperature.h"
#include "tools/stop_watch.h"
#include "esp_manager/esp_manager.h"

namespace etl 
{
    static uint32_t mem_free = ESP.getFreeHeap();   // контроль памяти 
    static uint32_t mem_free_before = mem_free;

    bool test_all(Stream& trace)
    {
        trace.println("--- Embedded Template Library ---");
        mem_free = ESP.getFreeHeap();
        
        test_result(trace, "test_empty", test_empty());
        
        test_result(trace, "test_optional", test_optional());
        test_result(trace, "test_unique", test_unique());
        test_result(trace, "test_queue", test_queue());
        test_result(trace, "test_vector", test_vector());
        test_result(trace, "test_array", test_array());
        test_result(trace, "test_espnow", test_espnow());
        test_result(trace, "test_lookup", test_lookup());

        test_result(trace, "test_empty", test_empty());
        
        trace.println("--------------------------------");
        int memory_leaks = int(ESP.getFreeHeap()) - mem_free;
        trace.print("MEMORY LEAKS: "); trace.print(memory_leaks); trace.println(memory_leaks == 0 ? "\tOK" : "\tFAILED");
        trace.println("--------------------------------");
     
    //    profiler_average_filter(trace);
    //    profiler_lookup_table(trace);

        return true;
    }

    
    #define TEST_EQUAL(actual, expected, error_message) if(actual != expected) return error_message;
    #define TEST_NOT_EQUAL(actual, expected, error_message) if(actual == expected) return error_message;
    
    void test_result(Stream& trace, const String& title, const String& result)
    {
        mem_free_before = mem_free;     mem_free = ESP.getFreeHeap();   // контроль памяти 
        int diff = int(mem_free_before) - int(mem_free);
        trace.print("MEMORY: "); trace.print(mem_free); trace.print(" diff = "); trace.print(diff); trace.print("\t| ");
        trace.print(title + " - ");  trace.println(result.isEmpty() ? "OK" : "FAILED: " + result);
    }

    String test_empty()
    {
        return "";  // no errors
    }

    String test_optional()
    {
        etl::optional<float> temperature;
        if(temperature) return "empty optinal";
        TEST_EQUAL(temperature.has_value(), false, "empty has_value");
        temperature = 36.6;
        TEST_EQUAL(math::equals(temperature.value(), 36.6), true, "<float> assign and check value");
        TEST_EQUAL(math::equals(temperature.value(), 36.66), false, "<float> compare 36.66");

        etl::optional<int> hum(100);
        TEST_EQUAL(hum.has_value(), true, "<int>(100) initialize");
        TEST_EQUAL(*hum, 100, "<int> emplace assign and check value");
        hum.reset();
        TEST_EQUAL(hum.has_value(), false, "reset failed");

        int value1 = hum.value_or(5); 
        TEST_EQUAL(hum.has_value(), false, "value_or(5)");
        TEST_EQUAL(value1, 5, "value_or(5) failed");

        return "";  // no errors
    }

    String test_unique()
    {
        etl::unique_ptr<int> int_ptr;
        if(int_ptr) return "empty unique_ptr failed";
        TEST_EQUAL(int_ptr.operator bool(), false, "operator bool()");
        TEST_EQUAL(int_ptr.empty(), true, "empty");
        TEST_EQUAL(int_ptr, nullptr, "nullptr compare");
        int_ptr = etl::make_unique<int>(42);
        if(!int_ptr) return "unique_ptr should be filled";
        TEST_NOT_EQUAL(int_ptr, nullptr, "not nullptr compare");
        TEST_EQUAL(*int_ptr, 42, "unique_ptr equils 42");
        return "";  // no errors
    }

    String test_queue()
    {
        etl::queue<int, 5> avgInt5; // Усреднение 5 значений
        TEST_EQUAL(avgInt5.empty(), true, "avgInt5.empty()");
        TEST_EQUAL(avgInt5.capacity(), 5, "avgInt5.capacity");
        for(int i = 1; i <= 5; ++i) avgInt5.push(i);
        TEST_EQUAL(avgInt5.full(), true, "avgInt5.full");
        TEST_EQUAL(avgInt5.size(), 5, "avgInt5.size 5");
        TEST_EQUAL(avgInt5.at(0), 1, "avgInt5.at(0)");
        TEST_EQUAL(avgInt5.at(1), 2, "avgInt5.at(1)");
        TEST_EQUAL(avgInt5.at(2), 3, "avgInt5.at(2)");
        TEST_EQUAL(avgInt5.at(3), 4, "avgInt5.at(3)");
        TEST_EQUAL(avgInt5.at(4), 5, "avgInt5.at(4)");

        int sum5 = 0; 
        for(auto value : avgInt5) sum5+= value;
        TEST_EQUAL(sum5, 15, "sum5 == 15");
        int avg5 = sum5 / avgInt5.size();
        TEST_EQUAL(avg5, 3, "avg5 == 3");

        TEST_EQUAL(avgInt5.push(6), false, "push to full queue");

        avgInt5.pop();
        TEST_EQUAL(avgInt5.full(), false, "pop one value");
        TEST_EQUAL(avgInt5.push(6), true, "push to not full queue");

        while(avgInt5.pop()) {}
        TEST_EQUAL(avgInt5.full(), false, "full() on empty queue");
        TEST_EQUAL(avgInt5.empty(), true, "empty queue");

        // Доступ по индексу (для медианного фильтра)
        avgInt5.clear();
        avgInt5.push(42);
        TEST_EQUAL(avgInt5[0], 42, "1: avgInt5.[0] 42)");
        TEST_EQUAL(avgInt5.front(), avgInt5.back(), "2: avgInt5.front() == back()");
        avgInt5.push(31);
        TEST_EQUAL(avgInt5[0], 42, "2: avgInt5.[0] 42)");
        TEST_EQUAL(avgInt5[1], 31, "2: avgInt5.[1] 31)");        
        TEST_EQUAL(avgInt5.front(), 42, "2: avgInt5.front() 42");
        TEST_EQUAL(avgInt5.back(), 31, "2: avgInt5.back() 31");
        avgInt5.push(20);
        TEST_EQUAL(avgInt5[0], 42, "3: avgInt5.[0] 42)");
        TEST_EQUAL(avgInt5[1], 31, "3: avgInt5.[1] 31)");
        TEST_EQUAL(avgInt5[2], 20, "3: avgInt5.[2] 20)");
        TEST_EQUAL(avgInt5.front(), 42, "3: avgInt5.front() 42");
        TEST_EQUAL(avgInt5.back(), 20, "3: avgInt5.back() 20");

        auto f1 = avgInt5.pop_front();
        TEST_EQUAL(f1.value(), 42, "4: avgInt5.pop_front() 42");
        auto f2 = avgInt5.pop_front();
        TEST_EQUAL(f2.value(), 31, "4: avgInt5.pop_front() 31");
        auto f3 = avgInt5.pop_front();
        TEST_EQUAL(f3.value(), 20, "4: avgInt5.pop_front() 20");

        TEST_EQUAL(avgInt5.empty(), true, "4: avgInt5.empty()");
        auto f4 = avgInt5.pop_front();
        TEST_EQUAL(f4.has_value(), false, "4: f4 empty has_value");

        return "";
    }

    String test_vector()
    {
        etl::vector<uint8_t> bytes;
        TEST_EQUAL(bytes.empty(), true, "vector empty");

        for(uint8_t i = 1; i <= 5; ++i ) bytes.push_back(i);
        TEST_EQUAL(bytes.empty(), false, "vector not empty");
        TEST_EQUAL(bytes.size(), 5, "vector size() 5");
        TEST_EQUAL(bytes.at(0), 1, "1: vector at(0) 1");
        TEST_EQUAL(bytes.at(1), 2, "1: vector at(1) 2");
        TEST_EQUAL(bytes.at(2), 3, "1: vector at(2) 3");
        TEST_EQUAL(bytes.at(3), 4, "1: vector at(3) 4");
        TEST_EQUAL(bytes.at(4), 5, "1: vector at(4) 5");

        bytes.clear();
        TEST_EQUAL(bytes.empty(), true, "2: vector empty");
        bytes.resize(20, 0xFF);
        TEST_EQUAL(bytes.size(), 20, "2: vector size() 20");
        TEST_EQUAL(bytes[10], 0xFF, "2: vector [10] 0xFF");
        bytes.reserve(30);
        TEST_EQUAL(bytes.capacity(), 30, "3: vector capacity() 30");
        TEST_EQUAL(bytes.size(), 20, "3: vector size() 20");
        TEST_EQUAL(bytes[10], 0xFF, "3: vector [10] 0xFF");        

        auto bytes_copy = bytes;
        TEST_EQUAL(bytes_copy.capacity(), bytes_copy.size(), "4: vector copy capacity() equl original size()");
        TEST_EQUAL(bytes_copy.size(), 20, "4: vector copy size() 20");
        TEST_EQUAL(bytes_copy[10], 0xFF, "4: vector copy [10] 0xFF");
        size_t capacity4 = bytes_copy.capacity();
        bytes_copy.push_back(0xAD);        
        TEST_EQUAL(bytes_copy.capacity(), capacity4 * 2, "5: vector copy capacity() increased x2");
        TEST_EQUAL(bytes_copy.size(), 21, "5: vector copy size() 20");
        TEST_EQUAL(bytes_copy.back(), 0xAD, "5: vector copy back() 0xAD");

        etl::vector<float> fv;
        float sum1 = 0;
        for(int i=0; i < 100; ++i) {
            float value = i * 2.0;
            fv.push_back(value);
            sum1 += value;
        }
        float sum2 = 0;
        for(const auto& value : fv) {
            sum2 += value;     
        }
        TEST_EQUAL(math::equals(sum1, sum2), true, "vector<float> assign and check summ");

        // Инициализация из статического массива
        const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
        etl::vector<uint8_t> mac (size_t(6), 0xFF);
        etl::vector<uint8_t> mac1 {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        etl::vector<uint8_t> mac2 {broadcastAddress}; 
        etl::vector<uint8_t> mac3 = broadcastAddress; 
        const etl::vector<uint8_t> mac4 {broadcastAddress}; 

        TEST_EQUAL(mac.size(), 6, "mac size() 6");
        TEST_EQUAL(mac.capacity(), 6, "mac capacity() 6");

        TEST_EQUAL(mac1.size(), 6, "mac1 size() 6");
        TEST_EQUAL(mac2.size(), 6, "mac2 size() 6");
        TEST_EQUAL(mac3.size(), 6, "mac3 size() 6");

        for(auto b : mac) TEST_EQUAL(b, 0xFF, "mac bytes oxFF");
        TEST_EQUAL(mac, mac1, "mac == mac1");
        TEST_EQUAL(mac1, mac2, "mac1 == mac2");
        TEST_EQUAL(mac2, mac3, "mac2 == mac3");
        TEST_EQUAL(mac3, mac4, "mac3 == mac4");
        
        return "";  // no errors
    }

    // PROGMEM массивы
    const uint8_t mac_address_p[] PROGMEM = {0xF4, 0xCF, 0xA2, 0x78, 0xDF, 0xF9};
    const uint16_t sensor_data_p[] PROGMEM = {100, 200, 300, 400, 500};
    const char welcome_text_p[] PROGMEM = "Hello from PROGMEM!";
   
    String test_array()
    {
        const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
        etl::array mac1{broadcastAddress};
        TEST_EQUAL(mac1.size(), 6, "array mac1 size() 6");
        uint32_t summ = 0;
        for(auto bb : mac1) summ += bb;
        TEST_EQUAL(summ, 6*255, "array mac1 summ");

        const uint8_t test_arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; 
        etl::array<uint8_t> sub1{test_arr1+2, size_t(3)};
        TEST_EQUAL(sub1.size(), 3, "array sub1 size() 3");
        TEST_EQUAL(sub1.at(0), 2, "array sub1.at(0) == 2");

        int test_arr2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; 
        etl::array<int> arr2 = test_arr2;
        auto it5 = arr2.find(5);
        TEST_NOT_EQUAL(it5, arr2.end(), "array fond 5: it5 != arr2.end()");
        etl::array<int> sub2{it5, size_t(5)};
        TEST_EQUAL(sub2.size(), 5, "array sub2 size() 5");
        TEST_EQUAL(sub2.at(0), 5, "array sub2.at(0) == 5");
        TEST_EQUAL(sub2.at(4), 9, "array sub2.at(4) == 9");

        // Создание оберток
        pgm::array mac_arr(mac_address_p);
        pgm::array sensor_arr(sensor_data_p);
        pgm::array text_arr(welcome_text_p);  // Для строк тоже работает!
    
        // Ranged for loop
        String s_mac = ("MAC: ");
        for (size_t i = 0; i < mac_arr.size(); ++i) {
            if (i > 0) s_mac += ":";
            s_mac += String(mac_arr[i], HEX);
        }
        s_mac.toUpperCase();
        TEST_EQUAL(s_mac, "MAC: F4:CF:A2:78:DF:F9", "pgm::array mac_arr(mac_address_p)" );
    
        // Доступ по индексу
        TEST_EQUAL(sensor_arr[0], 100, "First sensor value: 100");
        TEST_EQUAL(sensor_arr.size(), 5, "Sensor array size 5");
    
        // Поиск
        TEST_EQUAL(mac_arr.contains(0xA2), true, "MAC contains 0xA2");
           
        // Копирование в RAM
        uint8_t mac_ram[6];
        mac_arr.copy_to(mac_ram, sizeof(mac_ram));
        etl::array mem_arr(mac_ram);
        TEST_EQUAL(mac_arr.size(), mem_arr.size(), "mac_arr.size() == mem_arr.size()");
        TEST_EQUAL(mac_arr.at(3), mem_arr.at(3), "mac_arr.at(3) == mem_arr.at(3)");
    
        // Работа со строками
        String pgm_text = "Text: ";
        for (auto ch : text_arr) {
            if (ch == 0) break;  // null terminator
            pgm_text += String(static_cast<char>(ch));
        }
        TEST_EQUAL(pgm_text, "Text: Hello from PROGMEM!", "pgm_text = Text: Hello from PROGMEM!");
        String pgm_text2 = "Text: " + text_arr.to_string();
        TEST_EQUAL(pgm_text, pgm_text2, "pgm_text == pgm_text2");
        
        return "";  // no errors
    }

    String test_espnow()
    {
        const espnow::endpoint_t s001 {"F4:CF:A2:78:DF:F9"};    
        espnow::endpoint_t s002 {0xF4,0xCF,0xA2,0x78,0xDF,0xF9}; 
        
        const espnow::endpoint_t s003 {"EC:FA:BC:D5:A2:50"};
        
        TEST_NOT_EQUAL(s001, s003, "espnow s001 != s003");
        TEST_EQUAL(s001, s002, "espnow s001 == s002");

        const espnow::endpoint_t esp_modules[] = {s001, s002, s003}; 
        etl::array<espnow::endpoint_t> modules (esp_modules);
        TEST_EQUAL(modules.contains(s003), true, "espnow modules.contains(s003)");

        return "";  // no errors
    }

    const etl::lookup_t<float, float> ntc_sensor_3950_50K_p[] PROGMEM = {{1000.0, 1.0}, {2000.0, 2.0}, {3000.0, 3.0}};
    const etl::lookup_t<float, float> ntc_sensor_3950_50K[] = {{1000.0, 1.0}, {2000.0, 2.0}, {3000.0, 3.0}};
    const etl::lookup_t<float, float> ntc_sensor_3950_50K_desc[] = {{3000.0, 1.0}, {2000.0, 2.0}, {1000.0, 3.0}};
    String test_lookup()
    {
        etl::array ntc_temp(ntc_sensor_3950_50K);
        etl::array ntc_desc(ntc_sensor_3950_50K_desc);
        pgm::array ntc_temp_p(ntc_sensor_3950_50K_p);

        /////////////////////////////////////////////////////////
        //  ascending raw mode

        // Обрезка по границам с интерполяцией между значениями
        etl::lookup lt_ntc3950(ntc_temp);
        TEST_EQUAL(math::equals(lt_ntc3950.raw_to_value(1500), 1.5), true, "lt_ntc3950.raw_to_value(1500)");
        TEST_EQUAL(math::equals(lt_ntc3950.raw_to_value(500), 1.0), true, "lt_ntc3950.raw_to_value(500)");
        TEST_EQUAL(math::equals(lt_ntc3950.raw_to_value(3500), 3.0), true, "lt_ntc3950.raw_to_value(3500)");

        // С экстаполяцией за пределами крайних значений
        etl::lookup <float, float, lookup_mode::INTERPOLATE, bounds_mode::EXTRAPOLATE> lt_ntc3950_extra(ntc_temp);
        TEST_EQUAL(math::equals(lt_ntc3950_extra.raw_to_value(1500), 1.5), true, "lt_ntc3950_extra.raw_to_value(1500)");
        TEST_EQUAL(math::equals(lt_ntc3950_extra.raw_to_value(500), 0.5), true, "lt_ntc3950_extra.raw_to_value(500)");
        TEST_EQUAL(math::equals(lt_ntc3950_extra.raw_to_value(3500), 3.5), true, "lt_ntc3950_extra.raw_to_value(3500)");

        // Ближайшее значение без интеполяции с обрезкой
        etl::lookup <float, float, lookup_mode::NEAREST, bounds_mode::CLAMP> lt_ntc3950_near(ntc_temp);
        TEST_EQUAL(math::equals(lt_ntc3950_near.raw_to_value(1500), 1.0), true, "lt_ntc3950_near.raw_to_value(1500)");
        TEST_EQUAL(math::equals(lt_ntc3950_near.raw_to_value(1501), 2.0), true, "lt_ntc3950_near.raw_to_value(1501)");
        TEST_EQUAL(math::equals(lt_ntc3950_near.raw_to_value(500), 1.0), true, "lt_ntc3950_near.raw_to_value(500)");
        TEST_EQUAL(math::equals(lt_ntc3950_near.raw_to_value(3500), 3.0), true, "lt_ntc3950_near.raw_to_value(3500)");

    //    float temp = lt_ntc3950_near.raw_to_value(1501);
    //    Serial.println(temp,3);       

        /////////////////////////////////////////////////////////
        //  descending raw mode

        // Обрезка по границам с интерполяцией между значениями
        etl::lookup lt_ntc_desc(ntc_desc);
        TEST_EQUAL(math::equals(lt_ntc_desc.raw_to_value(1500), 2.5), true, "lt_ntc_desc.raw_to_value(1500)");
        TEST_EQUAL(math::equals(lt_ntc_desc.raw_to_value(500), 3.0), true, "lt_ntc_desc.raw_to_value(500)");
        TEST_EQUAL(math::equals(lt_ntc_desc.raw_to_value(3500), 1.0), true, "lt_ntc_desc.raw_to_value(3500)");

        // С экстаполяцией за пределами крайних значений
        etl::lookup <float, float, lookup_mode::INTERPOLATE, bounds_mode::EXTRAPOLATE> lt_ntc_desc_extra(ntc_desc);
        TEST_EQUAL(math::equals(lt_ntc_desc_extra.raw_to_value(1500), 2.5), true, "lt_ntc_desc_extra.raw_to_value(1500)");
        TEST_EQUAL(math::equals(lt_ntc_desc_extra.raw_to_value(500), 3.5), true, "lt_ntc_desc_extra.raw_to_value(500)");
        TEST_EQUAL(math::equals(lt_ntc_desc_extra.raw_to_value(3500), 0.5), true, "lt_ntc_desc_extra.raw_to_value(3500)");

        // Ближайшее значение без интеполяции с обрезкой
        etl::lookup <float, float, lookup_mode::NEAREST, bounds_mode::CLAMP> lt_ntc_desc_near(ntc_desc);
        TEST_EQUAL(math::equals(lt_ntc_desc_near.raw_to_value(1500), 2.0), true, "lt_ntc_desc_near.raw_to_value(1500)");
        TEST_EQUAL(math::equals(lt_ntc_desc_near.raw_to_value(1499), 3.0), true, "lt_ntc_desc_near.raw_to_value(1499)");
        TEST_EQUAL(math::equals(lt_ntc_desc_near.raw_to_value(1501), 2.0), true, "lt_ntc_desc_near.raw_to_value(1501)");
        TEST_EQUAL(math::equals(lt_ntc_desc_near.raw_to_value(500), 3.0), true, "lt_ntc_desc_near.raw_to_value(500)");
        TEST_EQUAL(math::equals(lt_ntc_desc_near.raw_to_value(3500), 1.0), true, "lt_ntc_desc_near.raw_to_value(3500)");
        
        return ""; // no errors 
    }

    void profiler_lookup_table(Stream& trace)
    {
        Serial.println();
        trace.println("--- PROFILER LOOKUP TABLE ---");
        const etl::lookup_t<float, float> ntc_sensor_3950_50K[] = {{1000.0, 1.0}, {2000.0, 2.0}, {3000.0, 3.0}};
        etl::array ntc_temp(ntc_sensor_3950_50K);
        
        etl::tools::stop_watch stat_arr_mem;
        etl::lookup lt_ntc3950(ntc_sensor_3950_50K /*ntc_temp*/);
        
        Serial.println("raw;\tT;\t");
        for(float raw = 1000; raw <= 3000; raw += 10)
        {
            Serial.print(raw,1); Serial.print(";\t");
            stat_arr_mem.start();
            float temp = lt_ntc3950.raw_to_value(raw);
            stat_arr_mem.stop();
            Serial.print(temp,4); Serial.print(";\t");
            Serial.println();
        }

        trace.println();
        trace.print("TIME;\tus;\t"); 
        trace.println(stat_arr_mem.get_averate_time()); 
    }
    
    // Функция для генерации редких выбросов
    float add_rare_noise(float base_value, int iteration) {
        // Генерируем выбросы примерно каждые 20-30 итераций
        if (iteration % random(20, 30) == 0 && iteration > 10) {
            // Большой выброс в положительную или отрицательную сторону
            return base_value + random(30, 60) * (random(0, 2) ? 1.0f : -1.0f);
        }
        // Иногда добавляем небольшие выбросы
        else if (random(0, 100) < 10 && iteration > 10) { // 10% chance
            return base_value + random(5, 15) * (random(0, 2) ? 1.0f : -1.0f);
        }
        return base_value;
    }

    void profiler_average_filter(Stream& trace)
    {
        ///////////////////////////////////////////////
        // скользящее окно на 5 целых чисел
        trace.println("--- PROFILER AVERAGE_FILETER ---");
        trace.println("i\tfilter_average<int, 5>");
        filter::moving_average<int, 5> avg5_int;
        for(float i = 0; i < 25; i += 1.0)
        {
            if(i == 15) { 
                avg5_int.reset();
                trace.println("reset");
            } 
            int avg = avg5_int.update(i);
            trace.print(i); trace.print("\t");
            trace.println(avg);
        }

        ///////////////////////////////////////////////
        // скользящее окно на 10 адщфе
        trace.println();
        trace.println("i\tfilter_average<float, 10>");
        filter::moving_average<float, 10> avg5_float;
        for(float i = 0; i < 25; i += 1.0)
        {
            if(math::equals(i, 15.0)) { // нифига не работает сравнение???
                avg5_float.reset();
                trace.println("reset");
            } 
            float avg = avg5_float.update(i);
            trace.print(i); trace.print("\t");
            trace.println(avg);
        }
        
        // Использование сложная структура для датчика температуры и влажности в бегущем среднем фильтра
        trace.println();
        trace.println("i;\tTraw;\tTm3;\tTavg;\tTexp;\tHraw;\tHm5;\tHavg;\tHexp;\t");
        
        // медианный фильтр на входе для удаления выбросов
        filter::median3<float> temperature_noice_remover; 
        filter::median5<float> humidity_noice_remover; 
        
        // Скользящее среднее
        filter::moving_average<sensor::data::th_t, 5> TH;
        // Экспоненциальный 
        filter::exponential<float> temperature_exp(0.3); // alpha = 0.1, 1.0 - без изменений
        filter::exponential<float> humidity_exp(0.5); // alpha = 0.1, 1.0 - без изменений
        sensor::data::th_t base{24.0f, 50};

        // Измерение скорости выполнения фильров и использование вектора для запоминания данных
        struct stats_t {
            etl::tools::stop_watch md3;
            etl::tools::stop_watch md5;
            etl::tools::stop_watch avg;
            etl::tools::stop_watch exp;
        } stats;        

        for(int i = 0; i < 200; ++i)
        {
            sensor::data::th_t raw = base + sensor::data::th_t{float(sin(i) * 2.0) + float(sin(i/50.0) * 30.0), float(sin(i) * 20.0)};
            raw.temperature = add_rare_noise(raw.temperature, i); // редкие выбросы для проверки медианных фильтров
            raw.humidity = add_rare_noise(raw.humidity, i); // редкие выбросы для проверки медианных фильтров
            //th_t median {raw};    // чтобы можно было убрать сглаживающий медианный фильтр
            
            stats.md3.start();
            float temperature_md3 = temperature_noice_remover.update(raw.temperature);
            stats.md3.stop();
            stats.md5.start();
            float humidity_md5 = humidity_noice_remover.update(raw.humidity);
            stats.md5.stop();
            sensor::data::th_t median {temperature_md3, humidity_md5};
            
            stats.avg.start();
            sensor::data::th_t avg = TH.update(median);
            stats.avg.stop();

            stats.exp.start();
            sensor::data::th_t exp {humidity_exp.update(median.temperature), temperature_exp.update(median.humidity)};
            stats.exp.stop();
            
            trace.print(i); trace.print(";\t");
            trace.print(raw.temperature); trace.print(";\t");
            trace.print(median.temperature); trace.print(";\t");
            trace.print(avg.temperature); trace.print(";\t");
            trace.print(exp.temperature); trace.print(";\t");
            trace.print(raw.humidity); trace.print(";\t");            
            trace.print(median.humidity); trace.print(";\t");  
            trace.print(avg.humidity); trace.print(";\t"); 
            trace.print(exp.humidity); trace.println(";\t");
        }

        // Статистика выполнения фильтров
        trace.println();
        trace.print("TIME;\tus;\t"); 
        trace.print(stats.md3.get_averate_time()); // Tm3
        trace.print(";\t");
        trace.print(stats.avg.get_averate_time()); // Tavg
        trace.print(";\t");
        trace.print(stats.exp.get_averate_time() / 2); // Texp
        trace.print(";\tus;\t");
        trace.print(stats.md5.get_averate_time()); // Hm5
        trace.print(";\t");
        trace.print(stats.avg.get_averate_time()); // Havg
        trace.print(";\t");
        trace.print(stats.exp.get_averate_time() / 2); // Hexp; 
        trace.print("\t");
        trace.println();

        // -------- WEMOS D1 MINI --------------------------------------------
        // TIME;   us;     4;      17;     4;      us;     14;     17;     4
        // median3 = 4us  vs median5 = 14u, md3 почти в 5 раз быстрее
        // -------- ESP32 C3 MINI --------------------------------------------
        // TIME;   us;     3;      7;      3;      us;     5;      7;      3
        // -------------------------------------------------------------------
    }

}


