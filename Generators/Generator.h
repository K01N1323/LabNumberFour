#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdexcept>
#include <functional>
#include "Sequence.h"
#include "Ordinal.h"

template <typename ItemType> class LazySequence;

template <typename ItemType>
struct Optional {
    bool HasValue;
    ItemType value;

    Optional() : HasValue(false), value(ItemType()) {}
    Optional(const ItemType& InitialValue) : HasValue(true), value(InitialValue) {}
};

template <typename ItemType>
class Generator {
private:
    LazySequence<ItemType>* owner; 
    std::function<ItemType(Sequence<ItemType>*)> rule;
    size_t position;

public:
    Generator(LazySequence<ItemType>* OwnerSequence, std::function<ItemType(Sequence<ItemType>*)> RuleFunction) 
        : owner(OwnerSequence), rule(RuleFunction), position(0) {}

    Generator(const Generator<ItemType>& other) 
        : owner(other.owner), rule(other.rule), position(other.position) {}

    ~Generator() = default;

    ItemType GetNext() {
        if (!HasNext()) {
            throw std::out_of_range("Достигнут конец последовательности генератора");
        }
        ItemType GeneratedValue = rule(owner); 
        position++; 
        return GeneratedValue; 
    }

    bool HasNext() const;

    Optional<ItemType> TryGetNext() {
        if (!HasNext()) {
            return Optional<ItemType>();
        }
        return Optional<ItemType>(GetNext());
    }

    void SetOwner(LazySequence<ItemType>* NewOwner) {
        owner = NewOwner;
    }

    // --- Делегированные операции мутаций ---

    Generator<ItemType>* Append(const ItemType& item) const {
        Generator<ItemType>* NextGenerator = new Generator<ItemType>(*this);
        if (this->owner->IsInfinite()) return NextGenerator; 
        
        auto OldRule = this->rule;
        int TargetPosition = this->owner->GetLengthOrdinal().GetCount();
        
        NextGenerator->rule = [OldRule, item, TargetPosition, ElementCounter = 0](Sequence<ItemType>* self) mutable -> ItemType {
            if (ElementCounter == TargetPosition) {
                ElementCounter++;
                return item; 
            }
            ElementCounter++;
            return OldRule(self); 
        };
        return NextGenerator;
    }

    Generator<ItemType>* InsertAt(const ItemType& item, int TargetIndex) const {
        Generator<ItemType>* NextGenerator = new Generator<ItemType>(*this);
        auto OldRule = this->rule;
        
        NextGenerator->rule = [OldRule, item, TargetIndex, ElementCounter = 0](Sequence<ItemType>* self) mutable -> ItemType {
            if (ElementCounter == TargetIndex) {
                ElementCounter++;
                return item; 
            }
            ElementCounter++;
            return OldRule(self); 
        };
        return NextGenerator;
    }

    Generator<ItemType>* RemoveAt(int TargetIndex) const {
        Generator<ItemType>* NextGenerator = new Generator<ItemType>(*this);
        auto OldRule = this->rule;
        
        NextGenerator->rule = [OldRule, TargetIndex, ElementCounter = 0](Sequence<ItemType>* self) mutable -> ItemType {
            if (ElementCounter == TargetIndex) {
                ElementCounter++;
                OldRule(self); // Вычисляем и отбрасываем удаленный элемент
            }
            ElementCounter++;
            return OldRule(self);
        };
        return NextGenerator;
    }
};

#endif // GENERATOR_H