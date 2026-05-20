#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include <stdexcept>
#include <functional>
#include "Sequence.h"
#include "MutableArraySequence.h"
#include "Generator.h"

struct Cardinal {
    bool IsInfinite;
    size_t size; 

    Cardinal(size_t value) : IsInfinite(false), size(value) {}
    Cardinal() : IsInfinite(true), size(0) {}

    bool IsIndexValid(int index) const {
        if (index < 0) return false;
        if (IsInfinite) return true; 
        return static_cast<size_t>(index) < size;
    }
};

template <typename T> 
class LazySequence : public Sequence<T> {
private:
    Generator<T>* gen;
    MutableArraySequence<T>* cache;
    Cardinal capacity;

public:
    LazySequence(std::function<T(Sequence<T>*)> func, Cardinal cap = Cardinal()) 
        : capacity(cap) {
        this->cache = new MutableArraySequence<T>();
        this->gen = new Generator<T>(this, func);
    }

    LazySequence(Generator<T>* ExistingGen, Cardinal cap) 
        : gen(ExistingGen), capacity(cap) {
        this->cache = new MutableArraySequence<T>();
    }

    virtual ~LazySequence() override {
        delete gen;
        delete cache;
    }

    const T& Get(int index) const override {
        if (index < 0 || !capacity.IsIndexValid(index)) {
            throw std::out_of_range("IndexOutOfRange: Индекс за пределами мощности");
        }

        while (cache->GetLength() <= index) {
            T NextValue = gen->GetNext();
            cache->Append(NextValue);
        }

        return cache->Get(index);
    }

    const T& GetFirst() const override {
        return this->Get(0);
    }

    const T& GetLast() const override {
        if (capacity.IsInfinite) {
            throw std::logic_error("У бесконечной последовательности нет последнего элемента");
        }
        if (capacity.size == 0) {
            throw std::out_of_range("Список пуст");
        }
        return this->Get(static_cast<int>(capacity.size) - 1);
    }

    int GetLength() const override {
        return capacity.IsInfinite ? -1 : static_cast<int>(capacity.size);
    }

    Cardinal GetCardinal() const {
        return capacity;
    }

    size_t GetMaterializedCount() const {
        return static_cast<size_t>(cache->GetLength());
    }

    Sequence<T>* Append(const T& item) override {
        Generator<T>* NewGen = gen->Append(item);
        Cardinal NewCap = capacity.IsInfinite ? capacity : Cardinal(capacity.size + 1);
        return new LazySequence<T>(NewGen, NewCap);
    }

    Sequence<T>* Prepend(const T& item) override {
        return this->InsertAt(item, 0);
    }

    Sequence<T>* InsertAt(const T& item, int index) override {
        if (index < 0 || (!capacity.IsInfinite && index > static_cast<int>(capacity.size))) { 
            throw std::out_of_range("Индекс вне размера массива");
        }

        auto InsertRule = [this, item, index, LocalPos = 0](Sequence<T>* self) mutable -> T {
            if (LocalPos == index) {
                LocalPos++;
                return item;
            }
            T val = this->Get(LocalPos > index ? LocalPos - 1 : LocalPos);
            LocalPos++;
            return val;
        };

        Cardinal NewCap = capacity.IsInfinite ? capacity : Cardinal(capacity.size + 1);
        return new LazySequence<T>(InsertRule, NewCap);
    }

    LazySequence<T>* Concat(LazySequence<T>* list) const {
        if (capacity.IsInfinite) {
            return new LazySequence<T>(*this); 
        }

        size_t FirstSize = capacity.size;
        Cardinal NewCap = list->capacity.IsInfinite ? Cardinal() : Cardinal(FirstSize + list->capacity.size);

        auto ConcatRule = [this, list, FirstSize, LocalPos = 0](Sequence<T>* self) mutable -> T {
            T value;
            if (LocalPos < static_cast<int>(FirstSize)) {
                value = this->Get(LocalPos);
            } else {
                value = list->Get(LocalPos - static_cast<int>(FirstSize));
            }
            LocalPos++; 
            return value;
        };

        return new LazySequence<T>(ConcatRule, NewCap);
    }

    template <typename T2>
    LazySequence<T2>* Map(T2 (*mapper)(const T&)) const {
        auto MapRule = [this, mapper, LocalPos = 0](Sequence<T2>* self) mutable -> T2 {
            T OriginalValue = this->Get(LocalPos);
            LocalPos++;
            return mapper(OriginalValue);
        };

        return new LazySequence<T2>(MapRule, this->capacity);
    }

    template <typename T2>
    T2 Reduce(T2 (*ReduceFunc)(const T2&, const T&), T2 StartValue) const {
        if (capacity.IsInfinite) {
            throw std::logic_error("Невозможно применить reduce для бесконечной последовательности");
        }

        T2 result = StartValue;
        for (int index = 0; index < static_cast<int>(capacity.size); index++) {
            result = ReduceFunc(result, this->Get(index));
        }
        return result;
    }

    LazySequence<T>* Where(bool (*predicate)(const T&)) const {
        auto FilterRule = [this, predicate, LocalPos = 0](Sequence<T>* self) mutable -> T {
            while (true) {
                if (!this->capacity.IsInfinite && LocalPos >= static_cast<int>(this->capacity.size)) {
                    throw std::out_of_range("Подходящих элементов больше нет");
                }
                
                T val = this->Get(LocalPos);
                LocalPos++;
                
                if (predicate(val)) {
                    return val; 
                }
            }
        };

        return new LazySequence<T>(FilterRule, this->capacity);
    }
};

#endif // LAZY_SEQUENCE_H