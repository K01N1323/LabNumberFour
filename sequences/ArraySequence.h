#ifndef ARRAYSEQUENCE_H
#define ARRAYSEQUENCE_H

#include <stdexcept>

#include "DynamicArray.h"
#include "Sequence.h"

// Абстрактный базовый класс для последовательностей на основе массива
template <class T> class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T> *items; // Указатель на внутренний динамический массив

public:
    // Конструктор по умолчанию: создает пустую последовательность
    ArraySequence() { 
        this->items = new DynamicArray<T>(0); 
    }

    // Конструктор копирования элементов из переданного массива
    ArraySequence(T *items, int count) {
        if (items == nullptr || count == 0) {
            this->items = new DynamicArray<T>(0);
        } else {
            this->items = new DynamicArray<T>(items, count);
        }
    }

    // Конструктор копирования
    ArraySequence(const ArraySequence<T> &other) {
        this->items = new DynamicArray<T>(*other.items);
    }

    // Деструктор: освобождает выделенную память
    virtual ~ArraySequence() override { 
        delete this->items; 
    }

    // Создает экземпляр текущего типа
    virtual ArraySequence<T> *Instance() override = 0;

    // Создает пустую последовательность текущего типа
    virtual ArraySequence<T> *CreateEmpty() const override = 0;

    // Возвращает первый элемент последовательности
    const T &GetFirst() const override { 
        return this->items->Get(0); 
    }

    // Возвращает последний элемент последовательности
    const T &GetLast() const override {
        return this->items->Get(this->items->GetSize() - 1);
    }

    // Получает элемент по индексу
    const T &Get(int index) const override { 
        return this->items->Get(index); 
    }

    // Возвращает длину последовательности
    int GetLength() const override { 
        return this->items->GetSize(); 
    }

    // Возвращает итератор (по умолчанию недоступно)
    IEnumerator<T> *GetEnumerator() const override { 
        return nullptr; 
    }

    // Добавляет элемент в конец последовательности
    Sequence<T> *Append(const T &item) override {
        ArraySequence<T> *target = this->Instance();
        
        target->items->Resize(target->items->GetSize() + 1);
        target->items->Set(target->items->GetSize() - 1, item);
        
        return target;
    }

    // Добавляет элемент в начало последовательности
    Sequence<T> *Prepend(const T &item) override {
        ArraySequence<T> *target = this->Instance();
        int size = target->items->GetSize();
        
        target->items->Resize(size + 1);
        
        for (int i = size; i > 0; i--) {
            target->items->Set(i, target->items->Get(i - 1));
        }
        
        target->items->Set(0, item);
        return target;
    }

    // Вставляет элемент по заданному индексу
    Sequence<T> *InsertAt(const T &item, int index) override {
        if (index < 0 || index > this->GetLength()) {
            throw std::out_of_range("Индекс невалиден");
        }
        
        ArraySequence<T> *target = this->Instance();
        int size = target->items->GetSize();
        
        target->items->Resize(size + 1);
        
        for (int i = size; i > index; i--) {
            target->items->Set(i, target->items->Get(i - 1));
        }
        
        target->items->Set(index, item);
        return target;
    }

    // Возвращает новую последовательность, являющуюся конкатенацией текущей и переданной
    Sequence<T> *Concat(Sequence<T> *list) override {
        Sequence<T> *result = this->CreateEmpty();
        
        for (int i = 0; i < this->GetLength(); i++) {
            Sequence<T> *old_ptr = result;
            result = result->Append(this->Get(i));
            if (result != old_ptr) delete old_ptr;
        }
        
        for (int i = 0; i < list->GetLength(); i++) {
            Sequence<T> *old_ptr = result;
            result = result->Append(list->Get(i));
            if (result != old_ptr) delete old_ptr;
        }
        
        return result;
    }

    // Возвращает подпоследовательность по заданным индексам
    Sequence<T> *GetSubsequence(int start_index, int end_index) const override {
        if (start_index < 0 || start_index >= this->GetLength() || end_index < 0 ||
            end_index >= this->GetLength() || start_index > end_index) {
            throw std::out_of_range("Индексы невалидны");
        }

        Sequence<T> *result = this->CreateEmpty();

        for (int index = start_index; index <= end_index; index++) {
            Sequence<T> *old_ptr = result;
            
            result = result->Append(this->Get(index));

            if (result != old_ptr) {
                delete old_ptr;
            }
        }
        
        return result;
    }
};

#endif // ARRAYSEQUENCE_H