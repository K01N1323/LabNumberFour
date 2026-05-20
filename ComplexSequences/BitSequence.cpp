#include "BitSequence.h"
#include <cstring>

// ===================== Bit =====================

// Конструктор по умолчанию или из значения bool
Bit::Bit(bool v) : value(v) {}

// Конструктор из int (преобразуется в логическое значение)
Bit::Bit(int v) : value(v != 0) {}

// Возвращает значение бита
bool Bit::GetValue() const { 
    return value; 
}

// Перегрузка оператора побитового И (&)
Bit Bit::operator&(const Bit &other) const {
    return Bit(this->value && other.value);
}

// Перегрузка оператора побитового ИЛИ (|)
Bit Bit::operator|(const Bit &other) const {
    return Bit(this->value || other.value);
}

// Перегрузка оператора побитового ИСКЛЮЧАЮЩЕГО ИЛИ (^)
Bit Bit::operator^(const Bit &other) const {
    return Bit(this->value != other.value);
}

// Перегрузка оператора логического отрицания (~)
Bit Bit::operator~() const { 
    return Bit(!this->value); 
}

// Перегрузка оператора сравнения на равенство (==)
bool Bit::operator==(const Bit &other) const { 
    return this->value == other.value; 
}

// Перегрузка оператора вывода в поток (<<)
std::ostream &operator<<(std::ostream &os, const Bit &b) { 
    os << b.value;
    return os;
}

// ===================== BitSequence =====================

// Конструктор по умолчанию
BitSequence::BitSequence() : data(nullptr), bit_count(0), byte_capacity(0) {}

// Конструктор копирования
BitSequence::BitSequence(const BitSequence &other) 
    : bit_count(other.bit_count), byte_capacity(other.byte_capacity) {
    if (byte_capacity > 0) {
        data = new uint8_t[byte_capacity];
        std::memcpy(data, other.data, byte_capacity);
    } else {
        data = nullptr;
    }
}

// Деструктор
BitSequence::~BitSequence() {
    delete[] data;
}

// Увеличивает ёмкость буфера при необходимости
void BitSequence::EnsureCapacity(int required_bytes) {
    if (required_bytes <= byte_capacity) return;

    int new_capacity = (byte_capacity == 0) ? 4 : byte_capacity * 2;
    if (new_capacity < required_bytes) new_capacity = required_bytes;

    uint8_t *new_data = new uint8_t[new_capacity];
    std::memset(new_data, 0, new_capacity);

    if (data != nullptr) {
        std::memcpy(new_data, data, byte_capacity);
        delete[] data;
    }

    data = new_data;
    byte_capacity = new_capacity;
}

// Добавляет бит в конец последовательности
void BitSequence::Append(const Bit &bit) {
    int byte_index = bit_count / 8;
    int bit_offset = bit_count % 8;

    // Обеспечиваем достаточную ёмкость
    EnsureCapacity(byte_index + 1);

    if (bit.GetValue()) {
        data[byte_index] |= (1 << bit_offset);   // Устанавливаем бит в 1 с помощью побитового ИЛИ
    }
    // Если бит == 0, ничего не нужно — EnsureCapacity уже обнуляет новые байты

    bit_count++;
}

// Возвращает бит по логическому индексу
Bit BitSequence::Get(int index) const {
    if (index < 0 || index >= bit_count) {
        throw std::out_of_range("Индекс бита невалиден");
    }

    int byte_index = index / 8;    // Вычисляем нужный байт
    int bit_offset = index % 8;    // Вычисляем сдвиг внутри байта

    return Bit((data[byte_index] >> bit_offset) & 1);  // Извлекаем бит с помощью побитового И
}

// Возвращает количество логических битов
int BitSequence::GetLength() const {
    return bit_count;
}

// Выполняет побитовое И с другой последовательностью битов
BitSequence* BitSequence::And(const BitSequence *other) const {
    BitSequence *result = new BitSequence();
    int len = (this->bit_count < other->bit_count) ? this->bit_count : other->bit_count;

    for (int i = 0; i < len; i++) {
        result->Append(this->Get(i) & other->Get(i));
    }

    return result;
}

// Выполняет побитовое ИЛИ с другой последовательностью битов
BitSequence* BitSequence::Or(const BitSequence *other) const {
    BitSequence *result = new BitSequence();
    int len = (this->bit_count < other->bit_count) ? this->bit_count : other->bit_count;

    for (int i = 0; i < len; i++) {
        result->Append(this->Get(i) | other->Get(i));
    }

    return result;
}

// Выполняет побитовое ИСКЛЮЧАЮЩЕЕ ИЛИ с другой последовательностью битов
BitSequence* BitSequence::Xor(const BitSequence *other) const {
    BitSequence *result = new BitSequence();
    int len = (this->bit_count < other->bit_count) ? this->bit_count : other->bit_count;

    for (int i = 0; i < len; i++) {
        result->Append(this->Get(i) ^ other->Get(i));
    }

    return result;
}

// Инвертирует все биты в последовательности
BitSequence* BitSequence::Not() const {
    BitSequence *result = new BitSequence();

    for (int i = 0; i < this->bit_count; i++) {
        result->Append(~this->Get(i));
    }

    return result;
}

// Перегрузка оператора вывода в поток
std::ostream &operator<<(std::ostream &os, const BitSequence &bs) {
    for (int i = 0; i < bs.bit_count; i++) {
        os << bs.Get(i).GetValue();
    }
    return os;
}
