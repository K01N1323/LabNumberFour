#ifndef BIT_SEQUENCE_H
#define BIT_SEQUENCE_H

#include <iostream>
#include <cstdint>
#include <stdexcept>

// Класс, представляющий отдельный бит (0 или 1)
class Bit {
private:
    bool value;

public:
    // Конструктор по умолчанию или из значения bool
    Bit(bool v = false);
    
    // Конструктор из int (преобразуется в логическое значение)
    Bit(int v);

    // Возвращает значение бита
    bool GetValue() const;

    // Перегрузка оператора побитового И (&)
    Bit operator&(const Bit &other) const;
    
    // Перегрузка оператора побитового ИЛИ (|)
    Bit operator|(const Bit &other) const;
    
    // Перегрузка оператора побитового ИСКЛЮЧАЮЩЕГО ИЛИ (^)
    Bit operator^(const Bit &other) const;
    
    // Перегрузка оператора логического отрицания (~)
    Bit operator~() const;

    // Перегрузка оператора сравнения на равенство (==)
    bool operator==(const Bit &other) const;

    // Перегрузка оператора вывода в поток (<<)
    friend std::ostream &operator<<(std::ostream &os, const Bit &b);
};

// Класс для работы с последовательностью битов (упакованное хранение: 8 бит на байт)
class BitSequence {
private:
    uint8_t *data;       // Буфер байтов для хранения упакованных битов
    int bit_count;       // Количество логических битов
    int byte_capacity;   // Выделенная ёмкость в байтах

    // Увеличивает ёмкость буфера при необходимости
    void EnsureCapacity(int required_bytes);

public:
    // Конструктор по умолчанию
    BitSequence();

    // Конструктор копирования
    BitSequence(const BitSequence &other);

    // Деструктор
    ~BitSequence();

    // Добавляет бит в конец последовательности
    void Append(const Bit &bit);

    // Возвращает бит по логическому индексу
    Bit Get(int index) const;

    // Возвращает количество логических битов
    int GetLength() const;

    // Выполняет побитовое И с другой последовательностью битов
    BitSequence *And(const BitSequence *other) const;
    
    // Выполняет побитовое ИЛИ с другой последовательностью битов
    BitSequence *Or(const BitSequence *other) const;
    
    // Выполняет побитовое ИСКЛЮЧАЮЩЕЕ ИЛИ с другой последовательностью битов
    BitSequence *Xor(const BitSequence *other) const;
    
    // Инвертирует все биты в последовательности
    BitSequence *Not() const;

    // Перегрузка оператора вывода в поток
    friend std::ostream &operator<<(std::ostream &os, const BitSequence &bs);
};

#endif // BIT_SEQUENCE_H