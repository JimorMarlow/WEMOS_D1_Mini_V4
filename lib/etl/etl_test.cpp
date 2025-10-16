#include "etl_test.h"
#include "etl_optional.h"
#include "etl_math.h"
#include "etl_memory.h"
#include "etl_queue.h"
#include "filter/moving_average.h"
#include "filter/exponential.h"
#include "filter/median.h"
#include "etl_utility.h"

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

        test_result(trace, "test_empty", test_empty());
        
        trace.println("--------------------------------");

        test_average_filter(trace);

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

    struct th_t {
        float temperature;
        float humidity;

        // Оператор сложения (создает новый объект)
        th_t operator+(const th_t& rhs) const {
            return th_t{temperature + rhs.temperature, humidity + rhs.humidity};
        }

        // Оператор деления на число (создает новый объект)
        th_t operator/(float x) const {
            return th_t{temperature / x, humidity / x};
        }

        th_t operator/(int x) const {
            return th_t{temperature / x, humidity / x};
        }

        th_t operator/(size_t x) const {
            return th_t{temperature / x, humidity / x};
        }

        // Оператор += (поэлементное прибавление к текущему объекту)
        th_t& operator+=(const th_t& rhs) {
            temperature += rhs.temperature;
            humidity += rhs.humidity;
            return *this;
        }

        th_t& operator-=(const th_t& rhs) {
            temperature -= rhs.temperature;
            humidity -= rhs.humidity;
            return *this;
        }
    };

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

    void test_average_filter(Stream& trace)
    {
        ///////////////////////////////////////////////
        // скользящее окно на 5 целых чисел
        trace.println();
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

        // Сложная структура для датчика температуры и влажности
        trace.println();
        trace.println("i;\tTraw;\tTm3;\tTavg;\tTexp;\tHraw;\tHm5;\tHavg;\tHexp;\t");
        
        // медианный фильтр на входе для удаления выбросов
        filter::median3<float> temperature_noice_remover; 
        filter::median5<float> humidity_noice_remover; 
        
        // Скользящее среднее
        filter::moving_average<th_t, 10> TH;
        // Экспоненциальный 
        filter::exponential<float> temperature_exp(0.3); // alpha = 0.1, 1.0 - без изменений
        filter::exponential<float> humidity_exp(0.5); // alpha = 0.1, 1.0 - без изменений
        th_t base{24.0f, 50};
        for(int i = 0; i < 200; ++i)
        {
            th_t raw = base + th_t{float(sin(i) * 2.0) + float(sin(i/50.0) * 30.0), float(sin(i) * 20.0)};
            raw.temperature = add_rare_noise(raw.temperature, i); // редкие выбросы для проверки медианных фильтров
            raw.humidity = add_rare_noise(raw.humidity, i); // редкие выбросы для проверки медианных фильтров
            //th_t median {raw};    // чтобы можно было убрать сглаживающий медианный фильтр
            th_t median {temperature_noice_remover.update(raw.temperature), humidity_noice_remover.update(raw.humidity)};
            th_t avg = TH.update(median);
            th_t exp {humidity_exp.update(median.temperature), temperature_exp.update(median.humidity)};
            
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
    }

}


