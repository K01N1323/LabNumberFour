#ifndef MUTABLEARRAYSEQUENCE_H
#define MUTABLEARRAYSEQUENCE_H

#include "ArraySequence.h"

// Изменяемая последовательность на основе массива
template <class T> class MutableArraySequence : public ArraySequence<T> {
public:
    // Конструктор по умолчанию
    MutableArraySequence() : ArraySequence<T>() {}
    
    // Конструктор: создает последовательность из массива элементов
    MutableArraySequence(T *items, int count) : ArraySequence<T>(items, count) {}
    
    // Конструктор копирования
    MutableArraySequence(const MutableArraySequence<T> &other) : ArraySequence<T>(other) {}

    // Возвращает текущий объект для изменения (без копирования)
    MutableArraySequence<T> *Instance() override { 
        return this; 
    }

    // Создает новую пустую изменяемую последовательность
    MutableArraySequence<T> *CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }
};

#endif // MUTABLEARRAYSEQUENCE_H