#pragma once
#include "Arduino.h"

/* STL подобный очередь с фиксированным размером, поддержка ranged for (DeepSeek edition)

Пример использования с итераторами:
cpp
void setup() {
    Serial.begin(9600);
    
    Serial.println("=== queue Iterator Test ===");
    
    queue<int, 5> queue;
    
    // Заполняем очередь
    for (int i = 1; i <= 5; i++) {
        queue.push(i * 10);
    }
    
    // Перебор с помощью range-based for loop
    Serial.println("Queue elements:");
    for (auto& item : queue) {
        Serial.println(item);
    }
    
    // Изменение элементов через итератор
    Serial.println("Modified elements:");
    for (auto& item : queue) {
        item += 5;  // Модифицируем каждый элемент
        Serial.println(item);
    }
    
    // Const итератор (только для чтения)
    Serial.println("Const iteration:");
    const queue<int, 5>& const_queue = queue;
    for (const auto& item : const_queue) {
        Serial.println(item);
    }
    
    // Смешанные операции
    Serial.println("Mixed operations:");
    queue.pop();  // Удаляем первый элемент
    queue.push(60);  // Добавляем новый
    
    for (auto& item : queue) {
        Serial.println(item);
    }
}

void loop() {
}
*/
namespace etl {

template<typename T, size_t MAX_SIZE = 16>
class queue {
private:
    T data[MAX_SIZE];
    size_t front_index = 0;
    size_t back_index = 0;
    size_t count = 0;

public:
    // Добавление в конец
    bool push(const T& value) {
        if (count >= MAX_SIZE) {
            return false;
        }
        
        data[back_index] = value;
        back_index = (back_index + 1) % MAX_SIZE;
        count++;
        return true;
    }
    
    // Удаление из начала
    bool pop() {
        if (empty()) {
            return false;
        }
        
        front_index = (front_index + 1) % MAX_SIZE;
        count--;
        return true;
    }
    
    // Просмотр первого элемента
    T& front() {
        return data[front_index];
    }
    
    const T& front() const {
        return data[front_index];
    }
    
    // Просмотр последнего элемента
    T& back() {
        return data[(back_index == 0) ? MAX_SIZE - 1 : back_index - 1];
    }
    
    const T& back() const {
        return data[(back_index == 0) ? MAX_SIZE - 1 : back_index - 1];
    }
    
    // Информация
    bool empty() const { 
        return count == 0; 
    }
    
    bool full() const { 
        return count == MAX_SIZE; 
    }
    
    size_t size() const { 
        return count; 
    }
    
    size_t capacity() const { 
        return MAX_SIZE; 
    }
    
    // Очистка
    void clear() {
        front_index = back_index = count = 0;
    }
    
    // Итератор для range-based for loop
    class Iterator {
    private:
        queue* queue_ = nullptr;
        size_t position;
        size_t visited;
        
    public:
        Iterator(queue* q, size_t pos, size_t vis = 0) 
            : queue_(q), position(pos), visited(vis) {}
        
        T& operator*() {
            return queue_->data[position];
        }
        
        Iterator& operator++() {
            position = (position + 1) % queue_->capacity();
            visited++;
            return *this;
        }
        
        bool operator!=(const Iterator& other) const {
            return position != other.position || visited != other.visited;
        }
    };
    
    // Константный итератор
    class ConstIterator {
    private:
        const queue* queue_ = nullptr;
        size_t position;
        size_t visited;
        
    public:
        ConstIterator(const queue* q, size_t pos, size_t vis = 0) 
            : queue_(q), position(pos), visited(vis) {}
        
        const T& operator*() const {
            return queue_->data[position];
        }
        
        ConstIterator& operator++() {
            position = (position + 1) % queue_->capacity();
            visited++;
            return *this;
        }
        
        bool operator!=(const ConstIterator& other) const {
            return position != other.position || visited != other.visited;
        }
    };
    
    // Методы для итераторов
    Iterator begin() {
        if (empty()) {
            return end();
        }
        return Iterator(this, front_index);
    }
    
    Iterator end() {
        return Iterator(this, back_index, count);
    }
    
    ConstIterator begin() const {
        if (empty()) {
            return end();
        }
        return ConstIterator(this, front_index);
    }
    
    ConstIterator end() const {
        return ConstIterator(this, back_index, count);
    }
};

} //namespace etl