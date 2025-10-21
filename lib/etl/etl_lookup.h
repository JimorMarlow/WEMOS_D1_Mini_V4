#pragma once
#include "Arduino.h"
#include "etl_array.h"

namespace etl {

// Lookup таблица с возможностью интерполяции значений между точками
template <typename T, typename V>
struct lookup_t
{
    T raw   = T{0};   // сырое показание датчика
    V value = V{0};   // физическое значение
};

// Перечисление для режимов поиска
enum class lookup_mode {
    NEAREST,      // Ближайшее значение
    INTERPOLATE   // Интерполяция между точками
};

// Перечисление для обработки границ
enum class bounds_mode {
    CLAMP,        // Брать значение границы
    EXTRAPOLATE   // Экстраполировать по двум крайним точкам
};

// Основной класс lookup таблицы
template <typename T, typename V, lookup_mode Mode = lookup_mode::INTERPOLATE, bounds_mode Bounds = bounds_mode::CLAMP>
class lookup {
private:
    etl::array<lookup_t<T, V>> table_;
    
    // Вспомогательные функции для сравнения
    static bool less_than(const T& a, const T& b) {
        return a < b;
    }
    
    static bool greater_than(const T& a, const T& b) {
        return a > b;
    }
    
    // Определяем направление сортировки
    bool is_ascending() const {
        if (table_.size() < 2) return true;
        return table_[0].raw < table_[1].raw;
    }
    
    // Бинарный поиск в отсортированном массиве
    size_t find_index(const T& raw) const {
        if (table_.empty()) return 0;
        
        size_t left = 0;
        size_t right = table_.size() - 1;
        
        if (is_ascending()) {
            while (left <= right) {
                size_t mid = left + (right - left) / 2;
                if (table_[mid].raw == raw) return mid;
                if (table_[mid].raw < raw) left = mid + 1;
                else right = mid - 1;
            }
            return left; // left указывает на позицию вставки
        } else {
            // для убывающего порядка
            while (left <= right) {
                size_t mid = left + (right - left) / 2;
                if (table_[mid].raw == raw) return mid;
                if (table_[mid].raw > raw) left = mid + 1;
                else right = mid - 1;
            }
            return left;
        }
    }
    
    // Интерполяция между двумя точками
    V interpolate(const lookup_t<T, V>& a, const lookup_t<T, V>& b, const T& raw) const {
        if (a.raw == b.raw) return a.value;
        
        // Для простых числовых типов
        if constexpr (std::is_arithmetic_v<V>) {
            double ratio = static_cast<double>(raw - a.raw) / static_cast<double>(b.raw - a.raw);
            return static_cast<V>(a.value + ratio * (b.value - a.value));
        } else {
            // Для сложных типов нужны определенные операторы
            return interpolate_custom(a, b, raw);
        }
    }
    
    // Интерполяция для пользовательских типов
    V interpolate_custom(const lookup_t<T, V>& a, const lookup_t<T, V>& b, const T& raw) const {
        // Пользователь должен определить операторы для своего типа V
        // или специализировать эту функцию
        double ratio = static_cast<double>(raw - a.raw) / static_cast<double>(b.raw - a.raw);
        return a.value + (b.value - a.value) * ratio;
    }

public:
    // Конструкторы
    lookup() = default;
    
    lookup(const etl::array<lookup_t<T, V>>& table) : table_(table) {}
    
    template<size_t N>
    lookup(const lookup_t<T, V> (&arr)[N]) : table_(arr) {}
    
    // Основная рабочая функция
    V raw_to_value(const T& raw) const {
        if (table_.empty()) return V{};
        if (table_.size() == 1) return table_[0].value;
        
        size_t index = find_index(raw);
        
        // Обработка границ
        if (index == 0) {
            // raw меньше первого элемента
            if constexpr (Bounds == bounds_mode::CLAMP) {
                return table_[0].value;
            } else { // EXTRAPOLATE
                if (table_.size() >= 2) {
                    return interpolate(table_[0], table_[1], raw);
                }
                return table_[0].value;
            }
        }
        
        if (index >= table_.size()) {
            // raw больше последнего элемента
            if constexpr (Bounds == bounds_mode::CLAMP) {
                return table_.back().value;
            } else { // EXTRAPOLATE
                if (table_.size() >= 2) {
                    return interpolate(table_[table_.size() - 2], table_.back(), raw);
                }
                return table_.back().value;
            }
        }
        
        // Точное совпадение
        if (table_[index].raw == raw) {
            return table_[index].value;
        }
        
        // Между двумя точками
        size_t prev_index = index - 1;
        
        if constexpr (Mode == lookup_mode::NEAREST) {
            // Ближайшее значение
            T dist_prev = std::abs(raw - table_[prev_index].raw);
            T dist_next = std::abs(raw - table_[index].raw);
            return (dist_prev <= dist_next) ? table_[prev_index].value : table_[index].value;
        } else { // INTERPOLATE
            // Интерполяция между точками
            return interpolate(table_[prev_index], table_[index], raw);
        }
    }
    
    // Получение минимального и максимального raw
    T min_raw() const {
        if (table_.empty()) return T{};
        if (is_ascending()) return table_[0].raw;
        return table_.back().raw;
    }
    
    T max_raw() const {
        if (table_.empty()) return T{};
        if (is_ascending()) return table_.back().raw;
        return table_[0].raw;
    }
    
    // Проверка на валидность
    bool valid() const {
        return !table_.empty();
    }
    
    // Размер таблицы
    size_t size() const {
        return table_.size();
    }
    
    // Доступ к таблице
    const etl::array<lookup_t<T, V>>& table() const {
        return table_;
    }
};

// Вспомогательные функции для создания lookup таблиц
template<lookup_mode Mode = lookup_mode::INTERPOLATE, 
         bounds_mode Bounds = bounds_mode::CLAMP,
         typename T, typename V, size_t N>
lookup<T, V, Mode, Bounds> make_lookup(const lookup_t<T, V> (&arr)[N]) {
    return lookup<T, V, Mode, Bounds>(arr);
}

template<lookup_mode Mode = lookup_mode::INTERPOLATE,
         bounds_mode Bounds = bounds_mode::CLAMP,
         typename T, typename V>
lookup<T, V, Mode, Bounds> make_lookup(const etl::array<lookup_t<T, V>>& arr) {
    return lookup<T, V, Mode, Bounds>(arr);
}

} // namespace etl

// Специализация для PROGMEM массивов
// cpp
// namespace etl {

// // Специализация для работы с PROGMEM массивами
// template <typename T, typename V, lookup_mode Mode = lookup_mode::INTERPOLATE, bounds_mode Bounds = bounds_mode::CLAMP>
// class pgm_lookup {
// private:
//     pgm::array<lookup_t<T, V>> table_;
    
//     // Чтение элемента из PROGMEM
//     lookup_t<T, V> read_element(size_t index) const {
//         lookup_t<T, V> result;
//         result.raw = table_[index].raw;   // автоматически читает из PROGMEM
//         result.value = table_[index].value; // автоматически читает из PROGMEM
//         return result;
//     }
    
//     // Бинарный поиск (аналогично обычному lookup)
//     size_t find_index(const T& raw) const {
//         if (table_.empty()) return 0;
        
//         size_t left = 0;
//         size_t right = table_.size() - 1;
//         bool ascending = true;
        
//         // Определяем направление сортировки
//         if (table_.size() >= 2) {
//             auto first = read_element(0);
//             auto second = read_element(1);
//             ascending = first.raw < second.raw;
//         }
        
//         if (ascending) {
//             while (left <= right) {
//                 size_t mid = left + (right - left) / 2;
//                 auto mid_elem = read_element(mid);
//                 if (mid_elem.raw == raw) return mid;
//                 if (mid_elem.raw < raw) left = mid + 1;
//                 else right = mid - 1;
//             }
//             return left;
//         } else {
//             while (left <= right) {
//                 size_t mid = left + (right - left) / 2;
//                 auto mid_elem = read_element(mid);
//                 if (mid_elem.raw == raw) return mid;
//                 if (mid_elem.raw > raw) left = mid + 1;
//                 else right = mid - 1;
//             }
//             return left;
//         }
//     }
    
//     // Интерполяция
//     V interpolate(const lookup_t<T, V>& a, const lookup_t<T, V>& b, const T& raw) const {
//         if (a.raw == b.raw) return a.value;
        
//         if constexpr (std::is_arithmetic_v<V>) {
//             double ratio = static_cast<double>(raw - a.raw) / static_cast<double>(b.raw - a.raw);
//             return static_cast<V>(a.value + ratio * (b.value - a.value));
//         } else {
//             double ratio = static_cast<double>(raw - a.raw) / static_cast<double>(b.raw - a.raw);
//             return a.value + (b.value - a.value) * ratio;
//         }
//     }

// public:
//     pgm_lookup() = default;
    
//     pgm_lookup(const pgm::array<lookup_t<T, V>>& table) : table_(table) {}
    
//     template<size_t N>
//     pgm_lookup(const lookup_t<T, V> (&arr)[N]) : table_(arr) {}
    
//     V raw_to_value(const T& raw) const {
//         if (table_.empty()) return V{};
//         if (table_.size() == 1) return read_element(0).value;
        
//         size_t index = find_index(raw);
        
//         // Обработка границ
//         if (index == 0) {
//             if constexpr (Bounds == bounds_mode::CLAMP) {
//                 return read_element(0).value;
//             } else {
//                 if (table_.size() >= 2) {
//                     auto first = read_element(0);
//                     auto second = read_element(1);
//                     return interpolate(first, second, raw);
//                 }
//                 return read_element(0).value;
//             }
//         }
        
//         if (index >= table_.size()) {
//             if constexpr (Bounds == bounds_mode::CLAMP) {
//                 return read_element(table_.size() - 1).value;
//             } else {
//                 if (table_.size() >= 2) {
//                     auto last = read_element(table_.size() - 1);
//                     auto prev = read_element(table_.size() - 2);
//                     return interpolate(prev, last, raw);
//                 }
//                 return read_element(table_.size() - 1).value;
//             }
//         }
        
//         auto current = read_element(index);
//         if (current.raw == raw) {
//             return current.value;
//         }
        
//         size_t prev_index = index - 1;
//         auto prev = read_element(prev_index);
        
//         if constexpr (Mode == lookup_mode::NEAREST) {
//             T dist_prev = std::abs(raw - prev.raw);
//             T dist_next = std::abs(raw - current.raw);
//             return (dist_prev <= dist_next) ? prev.value : current.value;
//         } else {
//             return interpolate(prev, current, raw);
//         }
//     }
    
//     bool valid() const { return !table_.empty(); }
//     size_t size() const { return table_.size(); }
// };

//} // namespace etl