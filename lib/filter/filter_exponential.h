#pragma once
// Экспоненциальный фильтр
// EMA (Exponential Moving Average)
// filtered_value = α * new_sample + (1 - α) * previous_filtered_value
// где:
// new_sample - новое входное значение
// previous_filtered_value - предыдущее отфильтрованное значение
// 
// α (alpha) - коэффициент сглаживания (0 < α ≤ 1)
// α ≈ 0.1 - очень сильное сглаживание, медленная реакция
// α ≈ 0.3 - умеренное сглаживание, баланс скорости и плавности
// α ≈ 0.7 - слабое сглаживание, быстрая реакция на изменения

#include "Arduino.h"
#include "filter_base.h"
#include "etl_optional.h"

template<typename T>
class filter_exponential : public filter_base<T>
{
private:
    T alpha = T{0};
    etl::optional<T> filtered_value;
    
public:
    
    filter_exponential(const T& smoothing_factor) 
        : alpha(constrain(smoothing_factor, 0.001, 1.0)) // Ограничиваем α в диапазоне (0, 1]        
        {}
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