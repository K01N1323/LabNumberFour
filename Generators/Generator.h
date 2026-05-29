#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdexcept>    
#include <functional>  
#include "sequences/Sequence.h"
#include "Ordinal/Ordinal.h"

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
    // владелец последовательность
    LazySequence<ItemType>* owner; 
    
    std::function<ItemType(Sequence<ItemType>*)> rule;
    
    // индекс который вычисляем при след. вызове
    size_t position;

public:
    Generator(LazySequence<ItemType>* OwnerSequence, std::function<ItemType(Sequence<ItemType>*)> RuleFunction) : owner(OwnerSequence), rule(RuleFunction), position(0) {}

    // при мутациях
    Generator(const Generator<ItemType>& other) : owner(other.owner), rule(other.rule), position(other.position) {}

    ~Generator() = default;

    // меняем овнера например при копировании
    void SetOwner(LazySequence<ItemType>* NewOwner) {
        owner = NewOwner; 
    }


    bool HasNext() const {
        if (owner == nullptr) return false; 
        
        return owner->IsInfinite() || (position < static_cast<size_t>(owner->GetLengthOrdinal().GetCount()));
    }

    ItemType GetNext() {
        if (!HasNext()) {
            throw std::out_of_range("Достигнут конец последовательности генератора");
        }
        
        ItemType GeneratedValue = rule(owner); 
        
        position++; 
        
        return GeneratedValue; 
    }

    Optional<ItemType> TryGetNext() {
        if (!HasNext()) {
            return Optional<ItemType>(); 
        }
        return Optional<ItemType>(GetNext()); 
    }
    // операции

    //аппенд
    Generator<ItemType>* Append(const ItemType& item) const {
    
        Generator<ItemType>* NextGenerator = new Generator<ItemType>(*this);
        NextGenerator->position = 0;
        
        if (this->owner->IsInfinite()) {
            throw std::logic_error("append для бесконечных последовательностей выпонляется на уровне LazySequence");
        }
        
        auto OriginalSeq = this->owner;               
        int TargetPosition = OriginalSeq->GetLengthOrdinal().GetCount(); 
        

        NextGenerator->rule = [OriginalSeq, item, TargetPosition, ElementCounter = 0](Sequence<ItemType>* self) mutable -> ItemType {

            if (ElementCounter == TargetPosition) {
                ElementCounter++;
                return item; 
            }
    
            ItemType val = OriginalSeq->Get(ElementCounter); 
            ElementCounter++;
            return val; 
        };
        return NextGenerator;
    }

    // инсерт
    Generator<ItemType>* InsertAt(const ItemType& item, int TargetIndex) const {
        Generator<ItemType>* NextGenerator = new Generator<ItemType>(*this);
        NextGenerator->position = 0;
        auto OriginalSeq = this->owner;
        
        NextGenerator->rule = [OriginalSeq, item, TargetIndex, ElementCounter = 0](Sequence<ItemType>* self) mutable -> ItemType {
    
            if (ElementCounter == TargetIndex) {
                ElementCounter++;
                return item; 
            }
            // делаем шаг назад если мы прошли индекс вставки
            int OriginalIndex = (ElementCounter > TargetIndex) ? ElementCounter - 1 : ElementCounter;
            ItemType val = OriginalSeq->Get(OriginalIndex);
            ElementCounter++;
            return val; 
        };
        return NextGenerator;
    }

    // ремув эт
    Generator<ItemType>* RemoveAt(int TargetIndex) const {
        Generator<ItemType>* NextGenerator = new Generator<ItemType>(*this);
        NextGenerator->position = 0;
        auto OriginalSeq = this->owner;
        
        NextGenerator->rule = [OriginalSeq, TargetIndex, ElementCounter = 0](Sequence<ItemType>* self) mutable -> ItemType {
            // идем на +1 если прошли эелемент удаления
            int OriginalIndex = (ElementCounter >= TargetIndex) ? ElementCounter + 1 : ElementCounter;
            ItemType val = OriginalSeq->Get(OriginalIndex);
            ElementCounter++;
            return val;
        };
        return NextGenerator;
    }
};


#endif // GENERATOR_H