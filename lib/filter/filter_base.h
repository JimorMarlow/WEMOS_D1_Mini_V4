// Базовые класс для фильтров
#pragma once

template<typename T>
class filter_base
{
public:
    filter_base() = default;
    virtual ~filter_base() = default;
    virtual T update(const T& value) { return value; }   // Добавить новое значение для фильтрации
    virtual void reset() {}                              // Сброс фильтра
};