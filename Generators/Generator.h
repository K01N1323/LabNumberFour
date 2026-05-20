#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdexcept>
#include <functional>
#include "Sequence.h"

template <typename T> class LazySequence;

template <typename T>
struct Optional {
    bool HasValue;
    T value;

    Optional() : HasValue(false), value(T()) {}
    Optional(const T& val) : HasValue(true), value(val) {}
};

template <typename T>
class Generator {
private:
    Sequence<T>* owner; 
    std::function<T(Sequence<T>*)> rule;
    size_t position;

    T* HistoryData;
    size_t HistoryCount;
    size_t MaxHistorySize;
    int VirtualSizeDelta; 

    void PushHistory(const T& item) {
        if (MaxHistorySize == 0) return;
        
        if (HistoryCount >= MaxHistorySize) {
            for (size_t index = 1; index < HistoryCount; index++) {
                HistoryData[index - 1] = HistoryData[index];
            }
            HistoryData[HistoryCount - 1] = item;
        } else {
            HistoryData[HistoryCount] = item;
            HistoryCount++;
        }
    }

public:
    Generator(Sequence<T>* OwnerSeq, std::function<T(Sequence<T>*)> RuleFunc, size_t MaxHistSize = 100) 
        : owner(OwnerSeq), rule(std::move(RuleFunc)), position(0), 
          HistoryCount(0), MaxHistorySize(MaxHistSize), VirtualSizeDelta(0) {
        this->HistoryData = new T[MaxHistorySize];
    }

    Generator(const Generator<T>& other) {
        this->owner = other.owner;
        this->rule = other.rule;
        this->position = other.position;
        this->HistoryCount = other.HistoryCount;
        this->MaxHistorySize = other.MaxHistorySize;
        this->VirtualSizeDelta = other.VirtualSizeDelta;

        this->HistoryData = new T[this->MaxHistorySize];
        for (size_t index = 0; index < this->HistoryCount; index++) {
            this->HistoryData[index] = other.HistoryData[index];
        }
    }

    ~Generator() {
        delete[] this->HistoryData;
    }

    const T& GetNext() {
        if (!HasNext()) {
            throw std::out_of_range("IndexOutOfRange: Достигнут конец последовательности");
        }

        T value = rule(owner); 
        PushHistory(value);
        position++; 
        return value;
    }

    bool HasNext() const {
        if (owner->capacity.IsInfinite) return true; 
        return position < (static_cast<size_t>(owner->GetLength()) + VirtualSizeDelta);
    }

    Optional<T> TryGetNext() {
        if (!HasNext()) {
            return Optional<T>();
        }
        return Optional<T>(GetNext());
    }

    Generator<T>* Append(const T& item) const {
        Generator<T>* NextGen = new Generator<T>(*this);
        if (this->owner->capacity.IsInfinite) {
            return NextGen; 
        }

        auto OldRule = this->rule;
        size_t TargetPos = static_cast<size_t>(this->owner->GetLength()) + this->VirtualSizeDelta;
        int LocalPos = static_cast<int>(this->position);

        NextGen->VirtualSizeDelta++;
        NextGen->rule = [OldRule, item, TargetPos, LocalPos](Sequence<T>* self) mutable -> T {
            if (LocalPos == static_cast<int>(TargetPos)) {
                LocalPos++;
                return item; 
            }
            LocalPos++;
            return OldRule(self); 
        };
        return NextGen;
    }

    Generator<T>* Append(Sequence<T>* items) const {
        Generator<T>* NextGen = new Generator<T>(*this);
        if (this->owner->capacity.IsInfinite || items == nullptr) {
            return NextGen;
        }

        auto OldRule = this->rule;
        size_t TargetPos = static_cast<size_t>(this->owner->GetLength()) + this->VirtualSizeDelta;
        int LocalPos = static_cast<int>(this->position);
        int ItemsCount = items->GetLength();

        NextGen->VirtualSizeDelta += ItemsCount;
        NextGen->rule = [OldRule, items, TargetPos, LocalPos, ItemsCount](Sequence<T>* self) mutable -> T {
            if (LocalPos >= static_cast<int>(TargetPos) && LocalPos < static_cast<int>(TargetPos) + ItemsCount) {
                T value = items->Get(LocalPos - static_cast<int>(TargetPos));
                LocalPos++;
                return value;
            }
            LocalPos++;
            return OldRule(self);
        };
        return NextGen;
    }

    Generator<T>* Insert(const T& item) const {
        Generator<T>* NextGen = new Generator<T>(*this);
        auto OldRule = this->rule;
        size_t TargetPos = this->position; 
        int LocalPos = static_cast<int>(this->position);

        NextGen->VirtualSizeDelta++;
        NextGen->rule = [OldRule, item, TargetPos, LocalPos](Sequence<T>* self) mutable -> T {
            if (LocalPos == static_cast<int>(TargetPos)) {
                LocalPos++;
                return item; 
            }
            LocalPos++;
            return OldRule(self); 
        };
        return NextGen;
    }

    Generator<T>* Insert(Sequence<T>* items) const {
        Generator<T>* NextGen = new Generator<T>(*this);
        if (items == nullptr) return NextGen;

        auto OldRule = this->rule;
        size_t TargetPos = this->position;
        int LocalPos = static_cast<int>(this->position);
        int ItemsCount = items->GetLength();

        NextGen->VirtualSizeDelta += ItemsCount;
        NextGen->rule = [OldRule, items, TargetPos, LocalPos, ItemsCount](Sequence<T>* self) mutable -> T {
            if (LocalPos >= static_cast<int>(TargetPos) && LocalPos < static_cast<int>(TargetPos) + ItemsCount) {
                T value = items->Get(LocalPos - static_cast<int>(TargetPos));
                LocalPos++;
                return value;
            }
            LocalPos++;
            return OldRule(self);
        };
        return NextGen;
    }

    Generator<T>* Remove(const T& item) const {
        Generator<T>* NextGen = new Generator<T>(*this);
        auto OldRule = this->rule;

        NextGen->rule = [OldRule, item](Sequence<T>* self) mutable -> T {
            while (true) {
                T value = OldRule(self);
                if (!(value == item)) { 
                    return value; 
                }
            }
        };
        return NextGen;
    }

    Generator<T>* Remove(Sequence<T>* items) const {
        Generator<T>* NextGen = new Generator<T>(*this);
        if (items == nullptr) return NextGen;

        auto OldRule = this->rule;

        NextGen->rule = [OldRule, items](Sequence<T>* self) mutable -> T {
            while (true) {
                T value = OldRule(self);
                bool found = false;
                for (int index = 0; index < items->GetLength(); index++) {
                    if (value == items->Get(index)) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return value;
                }
            }
        };
        return NextGen;
    }
};

#endif // GENERATOR_H