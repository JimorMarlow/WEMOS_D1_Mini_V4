#pragma once
// Медианный фильтр для 3 элементов для быстрой сортировки и удаления пиковых выбросов
// Median3 фильтр - оптимизированная версия для окна размером 3

#include "Arduino.h"
#include "filter_base.h"

template<typename T>
class filter_median3 : public filter_base<T>
{
private:
    T alpha = T{0};
    etl::optional<T> filtered_value;
    
public:
    
    filter_median3() {}
    virtual T update(const T& value) override
    {
        if (filtered_value) 
        {
            filtered_value = alpha * value + (1 - alpha) * filtered_value.value();
        }
        else
        {
            filtered_value = value;
            
        }  
        return *filtered_value;
    }

    virtual void reset() override {
         filtered_value.reset();
    }
};