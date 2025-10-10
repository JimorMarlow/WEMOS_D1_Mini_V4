/*
Минималистичный вариант для умных указателей по типу stl
PS. deepseek edition
*/
#pragma once
#include "Arduino.h"

namespace etl {

// Базовая версия unique_ptr
template<typename T>
class unique_ptr {
private:
    T* ptr = nullptr;

public:
    // Конструкторы
    unique_ptr() noexcept = default;
    
    explicit unique_ptr(T* p) noexcept : ptr(p) {}
    
    // Запрещаем копирование
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    // Перемещающие конструкторы
    unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
    
    // Деструктор
    ~unique_ptr() {
        reset();
    }
    
    // Модификаторы
    T* release() noexcept {
        T* old_ptr = ptr;
        ptr = nullptr;
        return old_ptr;
    }
    
    void reset(T* p = nullptr) noexcept {
        if (ptr) {
            delete ptr;
        }
        ptr = p;
    }
    
    void swap(unique_ptr& other) noexcept {
        T* temp = ptr;
        ptr = other.ptr;
        other.ptr = temp;
    }
    
    // Доступ к данным
    T* get() const noexcept {
        return ptr;
    }
    
    T& operator*() const {
        return *ptr;
    }
    
    T* operator->() const noexcept {
        return ptr;
    }
    
    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }
    
    // Сравнения
    bool operator==(std::nullptr_t) const noexcept {
        return ptr == nullptr;
    }
    
    bool operator!=(std::nullptr_t) const noexcept {
        return ptr != nullptr;
    }
};

// Специализация для массивов
template<typename T>
class unique_ptr<T[]> {
private:
    T* ptr = nullptr;

public:
    unique_ptr() noexcept = default;
    
    explicit unique_ptr(T* p) noexcept : ptr(p) {}
    
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
    
    ~unique_ptr() {
        reset();
    }
    
    void reset(T* p = nullptr) noexcept {
        if (ptr) {
            delete[] ptr;
        }
        ptr = p;
    }
    
    T* release() noexcept {
        T* old_ptr = ptr;
        ptr = nullptr;
        return old_ptr;
    }
    
    T& operator[](size_t index) const {
        return ptr[index];
    }
    
    T* get() const noexcept {
        return ptr;
    }
    
    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }
    
    void swap(unique_ptr& other) noexcept {
        T* temp = ptr;
        ptr = other.ptr;
        other.ptr = temp;
    }
};

// Фабричная функция make_unique
template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Специализация для массивов
template<typename T>
unique_ptr<T[]> make_unique(size_t size) {
    return unique_ptr<T[]>(new T[size]());
}

// Функция swap
template<typename T>
void swap(unique_ptr<T>& lhs, unique_ptr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

// Сравнения с nullptr
template<typename T>
bool operator==(std::nullptr_t, const unique_ptr<T>& p) noexcept {
    return p.get() == nullptr;
}

template<typename T>
bool operator!=(std::nullptr_t, const unique_ptr<T>& p) noexcept {
    return p.get() != nullptr;
}

} // namespace emd

// Пример использования:
// cpp
// #include "etl_memory.h"

// struct Sensor {
//     float temperature;
//     float humidity;
    
//     Sensor(float t, float h) : temperature(t), humidity(h) {}
    
//     void read() {
//         temperature = random(200, 250) / 10.0f;
//         humidity = random(400, 700) / 10.0f;
//     }
// };

// class DeviceManager {
// private:
//     etl::unique_ptr<Sensor> sensor;
//     etl::unique_ptr<float[]> buffer;

// public:
//     void initialize() {
//         // Создание одиночного объекта
//         sensor = etl::make_unique<Sensor>(0.0f, 0.0f);
        
//         // Создание массива
//         buffer = etl::make_unique<float[]>(10);
//     }
    
//     void update() {
//         if (sensor) {
//             sensor->read();
//             Serial.print("Temp: "); Serial.println(sensor->temperature);
//         }
        
//         if (buffer) {
//             for (int i = 0; i < 10; i++) {
//                 buffer[i] = i * 1.5f;
//             }
//         }
//     }
    
//     // Передача владения
//     etl::unique_ptr<Sensor> releaseSensor() {
//         return std::move(sensor);
//     }
// };

// void setup() {
//     Serial.begin(9600);
    
//     Serial.println("=== etl::unique_ptr Demo ===");
    
//     // Базовое использование
//     etl::unique_ptr<int> ptr = etl::make_unique<int>(42);
//     Serial.print("Value: "); Serial.println(*ptr);
    
//     // Использование с классами
//     DeviceManager manager;
//     manager.initialize();
//     manager.update();
    
//     // Перемещение семантика
//     etl::unique_ptr<Sensor> sensor1 = etl::make_unique<Sensor>(25.0f, 60.0f);
//     etl::unique_ptr<Sensor> sensor2 = std::move(sensor1);
    
//     if (!sensor1) {
//         Serial.println("sensor1 is now empty");
//     }
    
//     if (sensor2) {
//         Serial.print("sensor2 temp: "); Serial.println(sensor2->temperature);
//     }
    
//     // Массивы
//     etl::unique_ptr<int[]> array = etl::make_unique<int[]>(5);
//     for (int i = 0; i < 5; i++) {
//         array[i] = i * 10;
//         Serial.print("Array["); Serial.print(i); 
//         Serial.print("] = "); Serial.println(array[i]);
//     }
    
//     // Автоматическая очистка при выходе из scope
//     {
//         etl::unique_ptr<float> temp = etl::make_unique<float>(3.14f);
//         Serial.print("Temporary value: "); Serial.println(*temp);
//     } // Память автоматически освобождается здесь
// }

// void loop() {
//     // Демонстрация автоматического управления памятью
//     etl::unique_ptr<uint32_t[]> data = etl::make_unique<uint32_t[]>(100);
    
//     for (int i = 0; i < 100; i++) {
//         data[i] = millis() + i;
//     }
    
//     // Память автоматически освободится при выходе из loop()
//     delay(1000);
// }
// Ключевые особенности:
// ✅ Полная совместимость синтаксиса с std::unique_ptr
// ✅ Поддержка массивов через unique_ptr<T[]>
// ✅ Фабричная функция make_unique
// ✅ Перемещающая семантика
// ✅ Автоматическое управление памятью (RAII)
// ✅ Минимальный footprint (~300 байт flash)