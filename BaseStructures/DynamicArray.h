#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdexcept>

// Класс для представления динамического массива
template <class T> class DynamicArray {
private:
    T *items; // Указатель на элементы массива
    int size; // Текущий размер массива

public:
    // Конструктор: создает массив заданного размера
    DynamicArray(int size) : DynamicArray(nullptr, size) {}

    // Конструктор: проверяет и инициализирует массив переданными элементами
    DynamicArray(T *items, int count) {
        this->size = count;
        this->items = new T[size];

        if (items == nullptr) {
            for (int element = 0; element < size; element++) {
                this->items[element] = T(0);
            }
        } else {
            for (int element = 0; element < size; element++) {
                this->items[element] = items[element];
            }
        }
    }

    // Конструктор копирования
    DynamicArray(const DynamicArray<T> &dynamic_array) {
        this->size = dynamic_array.size;
        this->items = new T[size];

        for (int element = 0; element < size; element++) {
            items[element] = dynamic_array.items[element];
        }
    }

    // Возвращает размер массива
    int GetSize() const { 
        return this->size; 
    }

    // Получает элемент по индексу
    const T &Get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Индекс невалиден");
        }
        
        return items[index];
    }

    // Устанавливает значение элемента по индексу
    void Set(int index, const T &value) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Индекс невалиден");
        }
        
        items[index] = value;
    }

    // Изменяет размер массива
    void Resize(int new_size) {
        T *new_items = new T[new_size];
        int elements_to_copy = (new_size < size) ? new_size : size;
        
        for (int element = 0; element < elements_to_copy; element++) {
            new_items[element] = items[element];
        }
        
        delete[] items;
        this->size = new_size;
        this->items = new_items;
    }

    // Деструктор: освобождает выделенную память
    ~DynamicArray() { 
        delete[] items; 
    }
};

#endif // DYNAMICARRAY_H