#pragma once
// Медианный фильтр для 3 элементов для быстрой сортировки и удаления пиковых выбросов
// Median3 фильтр - оптимизированная версия для окна размером 3

#include "Arduino.h"
#include "filter/base.h"
#include "etl_queue.h"

namespace etl
{
namespace filter
{
template<typename T>
class median3 : public base<T>
{
private:
    etl::queue<T, 3> _values;
    
public:
    median3() = default;

    static T fast_median(T a, T b, T c) // специально работаю с копиями объектов для упрощения перестановок
    {
        // Сортировка сети для 3 элементов
        if (a > b) std::swap(a, b); // теперь a <= b
        if (b > c) std::swap(b, c); // теперь b <= c, c - максимальное
        if (a > b) std::swap(a, b); // теперь a <= b
        // Медиана - средний элемент b
        return b;
    }
    
    virtual T update(const T& value) override
    {
        if(_values.full()) _values.pop();
        _values.push(value);   
        if(!_values.full())
        {
            if(_values.size() == 1)  { // первое значение - накапливаем и возвращаем сырые данные
                return value;   
            }
            if(_values.size() == 2) {  // отдаем среднее первых двух    
                return static_cast<T>((_values[0] + _values[1]) / 2); 
            }
        }        
        // Фильтрация по трем значениям 
        return fast_median(_values[0], _values[1], _values[2]);        
    }

    virtual void reset() override {
         _values.clear();
    }
};

}// namespace filter
}// namespace etl