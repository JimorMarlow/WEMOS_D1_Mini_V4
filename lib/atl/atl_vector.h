#pragma once
#include "Arduino.h"

/* STL подобный динамический вектор с минималистичной реализацией (DeepSeek edition)
Пример использования:

void setup() {
    Serial.begin(9600);
    
    // Версия с начальной емкостью 4
    emb::vector<int, 4> numbers;
    
    Serial.println("=== vector Demo ===");
    
    // Добавляем элементы
    for(int i = 0; i < 10; i++) {
        numbers.push_back(i * 10);
        Serial.print("Added: "); Serial.print(i * 10);
        Serial.print(", Size: "); Serial.print(numbers.size());
        Serial.print(", Capacity: "); Serial.println(numbers.capacity());
    }
    
    // Доступ к элементам
    Serial.println("\nElements:");
    for(size_t i = 0; i < numbers.size(); i++) {
        Serial.println(numbers[i]);
    }
    
    // Использование итераторов
    Serial.println("\nUsing iterators:");
    for(auto& num : numbers) {
        Serial.println(num);
    }
    
    // Тестирование статистики
    DynamicVectorWithStats<float, 2> stats_vec;
    for(int i = 0; i < 8; i++) {
        stats_vec.push_back(i * 1.5f);
    }
    
    Serial.print("\nMemory usage: "); 
    Serial.print(stats_vec.getMemoryUsage()); 
    Serial.println(" bytes");
    Serial.print("Max size: "); Serial.println(stats_vec.getMaxSize());
}

void loop() {}
*/

namespace emb
{
template<typename T, size_t INITIAL_CAPACITY = 8>
class vector {
private:
    T* data = nullptr;
    size_t _capacity = 0;
    size_t _size = 0;
    
    void resize(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for(size_t i = 0; i < _size; i++) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        _capacity = new_capacity;
    }
    
public:
    // Конструкторы
    vector() {
        reserve(INITIAL_CAPACITY);
    }
    
    vector(size_t initial_size) : _size(initial_size) {
        reserve(initial_size > INITIAL_CAPACITY ? initial_size : INITIAL_CAPACITY);
    }
    
    // Деструктор
    ~vector() {
        if(data) delete[] data;
    }
    
    // Копирующий конструктор
    vector(const vector& other) {
        reserve(other._capacity);
        _size = other._size;
        for(size_t i = 0; i < _size; i++) {
            data[i] = other.data[i];
        }
    }
    
    // Оператор присваивания
    vector& operator=(const vector& other) {
        if(this != &other) {
            clear();
            reserve(other._capacity);
            _size = other._size;
            for(size_t i = 0; i < _size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    // Добавление элементов
    void push_back(const T& value) {
        if(_size >= _capacity) {
            resize(_capacity == 0 ? INITIAL_CAPACITY : _capacity * 2);
        }
        data[_size++] = value;
    }
    
    void pop_back() {
        if(_size > 0) _size--;
    }
    
    // Доступ к элементам
    T& operator[](size_t index) { 
        return data[index]; 
    }
    
    const T& operator[](size_t index) const { 
        return data[index]; 
    }
    
    T& at(size_t index) {
        if(index >= _size) {
            // Простая обработка ошибки для embedded
            static T dummy;
            return dummy;
        }
        return data[index];
    }
    
    T& front() { return data[0]; }
    T& back() { return data[_size - 1]; }
    
    const T& front() const { return data[0]; }
    const T& back() const { return data[_size - 1]; }
    
    // Информация
    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }
    
    // Управление памятью
    void reserve(size_t new_capacity) {
        if(new_capacity > _capacity) {
            resize(new_capacity);
        }
    }
    
    // void resize(size_t new_size, const T& value = T()) {
    //     if(new_size > _capacity) {
    //         reserve(new_size);
    //     }
    //     for(size_t i = _size; i < new_size; i++) {
    //         data[i] = value;
    //     }
    //     _size = new_size;
    // }
    
    void clear() {
        _size = 0;
    }
    
    void shrink_to_fit() {
        if(_size < _capacity) {
            resize(_size);
        }
    }
    
    // Итераторы (простая реализация)
    T* begin() { return data; }
    T* end() { return data + _size; }
    const T* begin() const { return data; }
    const T* end() const { return data + _size; }
};
}