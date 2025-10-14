#include "etl_test.h"
#include "etl_optional.h"
#include "etl_math.h"
#include "etl_memory.h"
#include "etl_queue.h"

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

        return "";
    }


    template<typename T, size_t MAX_SIZE = 5>
    class filter_average
    {
    private:
        etl::queue<T, MAX_SIZE> _values;
        T _summ = T{0};//static_cast<T>{0};        
    public:
        T proccess(const T& value)
        {
            if(_values.full()) {
                _summ -= _values.front();
                _values.pop();
            }
            if(_values.push(value)) {
                _summ += value;
            }
            return _summ / _values.size();
        }

        void reset() {
            _values.clear();
            _summ = static_cast<T>(0);        
        }
    };

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


    void test_average_filter(Stream& trace)
    {
        trace.println();
        trace.println("i\tfilter_average<int, 5>");
        filter_average<float, 5> avg5_int;
        for(float i = 0; i < 25; i += 1.0)
        {
            if(math::equals(i, 15.0)) { // нифига не работает сравнение???
                avg5_int.reset();
                trace.println("reset");
            } 
            float avg = avg5_int.proccess(i);
            trace.print(i); trace.print("\t");
            trace.println(avg);
        }

        trace.println();
        trace.println("i\tfilter_average<th_t, 5>");
        filter_average<th_t, 5> TH;
        th_t base{24.0f, 50};
        for(int i = 0; i < 20; ++i)
        {
            base = base + th_t{float(sin(i) * 2.0), 50};
            th_t avg = TH.proccess(base);
            trace.print(i); trace.print("\t");
            trace.print(avg.temperature); trace.print("C\t");
            trace.print(avg.humidity); trace.println("%");
        }
    }

}


