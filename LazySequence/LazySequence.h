#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include <stdexcept>
#include <functional>
#include <utility>
#include "Sequence.h"
#include "MutableArraySequence.h"
#include "Generator.h"
#include "Ordinal.h"

template <typename ItemType> 
class LazySequence : public Sequence<ItemType> {
private:
    Generator<ItemType>* generator;
    MutableArraySequence<ItemType>* cache;
    Ordinal capacity; 
    
    // Массив чанков для реализации алгебры ординалов (Concat)
    MutableArraySequence<LazySequence<ItemType>*>* ChunkList; 

    // Приватный конструктор для Concat (создает роутер чанков)
    LazySequence(MutableArraySequence<LazySequence<ItemType>*>* CombinedChunks) 
        : generator(nullptr), cache(nullptr), ChunkList(CombinedChunks) {
        
        int TotalOmega = 0;
        int TotalIndex = 0;
        for (int ChunkIndex = 0; ChunkIndex < ChunkList->GetLength(); ChunkIndex++) {
            Ordinal ChunkCapacity = ChunkList->Get(ChunkIndex)->GetLengthOrdinal();
            TotalOmega += ChunkCapacity.omega;
            TotalIndex += ChunkCapacity.index;
        }
        this->capacity = Ordinal(TotalOmega, TotalIndex);
    }

    // Приватный конструктор для мутаций (Append, InsertAt и тд)
    LazySequence(Generator<ItemType>* ExistingGenerator, Ordinal NewCapacity) 
        : generator(ExistingGenerator), cache(new MutableArraySequence<ItemType>()), capacity(NewCapacity) {
        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
        if (this->generator != nullptr) {
            this->generator->SetOwner(this);
        }
    }

public:
    LazySequence(std::function<ItemType(Sequence<ItemType>*)> RuleFunction, Ordinal InitialCapacity) 
        : cache(new MutableArraySequence<ItemType>()), capacity(InitialCapacity) {
        this->generator = new Generator<ItemType>(this, RuleFunction);
        
        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
    }

    LazySequence() 
        : generator(nullptr), cache(new MutableArraySequence<ItemType>()), capacity(Ordinal(0, 0)) {
        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
    }

    LazySequence(ItemType* items, int count) 
        : cache(new MutableArraySequence<ItemType>()), capacity(Ordinal(0, count)) {
        for (int ElementIndex = 0; ElementIndex < count; ElementIndex++) {
            this->cache->Append(items[ElementIndex]);
        }
        
        auto ArrayRule = [this, ElementCounter = 0](Sequence<ItemType>* self) mutable -> ItemType {
            ItemType FetchedValue = this->cache->Get(ElementCounter);
            ElementCounter++;
            return FetchedValue;
        };
        this->generator = new Generator<ItemType>(this, ArrayRule);
        
        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
    }

    LazySequence(const LazySequence<ItemType>& other) 
        : capacity(other.capacity) {
        this->cache = new MutableArraySequence<ItemType>();
        for (int CacheIndex = 0; CacheIndex < other.cache->GetLength(); CacheIndex++) {
            this->cache->Append(other.cache->Get(CacheIndex));
        }
        
        if (other.generator != nullptr) {
            this->generator = new Generator<ItemType>(*other.generator);
            this->generator->SetOwner(this); 
        } else {
            this->generator = nullptr;
        }

        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
    }

    virtual ~LazySequence() override {
        delete generator;
        delete cache;
        delete ChunkList; 
    }
    
    bool IsInfinite() const { return capacity.IsInfinite(); }

    Ordinal GetLengthOrdinal() const { return capacity; }

    int GetLength() const override {
        if (IsInfinite()) throw std::out_of_range("У бесконечной последовательности нет конечной длины");
        return capacity.GetCount();
    }

    size_t GetMaterializedCount() const {
        return cache ? cache->GetLength() : 0;
    }

    // --- ДОСТУП К ЭЛЕМЕНТАМ (с поддержкой ординалов) ---

    const ItemType& Get(Ordinal TargetOrdinal) const {
        // Если это результат Concat (роутер)
        if (generator == nullptr && cache == nullptr) {
            if (TargetOrdinal.omega < 0 || TargetOrdinal.omega >= ChunkList->GetLength()) {
                throw std::out_of_range("Дельта (чанк) вне диапазона конкатенации");
            }
            return ChunkList->Get(TargetOrdinal.omega)->Get(Ordinal(0, TargetOrdinal.index)); 
        }

        // Если это базовая последовательность
        if (TargetOrdinal.omega > 0) throw std::logic_error("Дельта > 0 недопустима для одиночного чанка");
        if (!capacity.IsInfinite() && TargetOrdinal.index >= capacity.GetCount()) {
            throw std::out_of_range("Индекс выходит за границы");
        }

        while (cache->GetLength() <= TargetOrdinal.index) {
            if (generator == nullptr) throw std::logic_error("Генератор не инициализирован");
            ItemType GeneratedValue = generator->GetNext();
            const_cast<LazySequence<ItemType>*>(this)->cache->Append(GeneratedValue);
        }
        return cache->Get(TargetOrdinal.index);
    }

    const ItemType& Get(int TargetIndex) const override {
        return Get(Ordinal(0, TargetIndex)); 
    }

    const ItemType& GetFirst() const override { return this->Get(Ordinal(0, 0)); }

    const ItemType& GetLast() const override {
        if (IsInfinite()) throw std::logic_error("Нельзя получить последний элемент бесконечной структуры");
        if (generator == nullptr && cache == nullptr) { 
            LazySequence<ItemType>* LastChunk = ChunkList->Get(ChunkList->GetLength() - 1);
            return LastChunk->Get(Ordinal(0, LastChunk->GetLengthOrdinal().GetCount() - 1));
        }
        if (capacity.GetCount() == 0) throw std::out_of_range("Последовательность пуста");
        return this->Get(Ordinal(0, capacity.GetCount() - 1));
    }

    // --- КОНКАТЕНАЦИЯ И МУТАЦИИ ---

    LazySequence<ItemType>* Concat(LazySequence<ItemType>* OtherSequence) const {
        MutableArraySequence<LazySequence<ItemType>*>* NewChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        
        for (int ChunkIndex = 0; ChunkIndex < this->ChunkList->GetLength(); ChunkIndex++) {
            NewChunkList->Append(this->ChunkList->Get(ChunkIndex));
        }
        for (int OtherChunkIndex = 0; OtherChunkIndex < OtherSequence->ChunkList->GetLength(); OtherChunkIndex++) {
            NewChunkList->Append(OtherSequence->ChunkList->Get(OtherChunkIndex));
        }
        
        return new LazySequence<ItemType>(NewChunkList);
    }

    Sequence<ItemType>* Append(const ItemType& item) override {
        if (generator == nullptr) throw std::logic_error("Мутации возможны только на базовых последовательностях");
        Generator<ItemType>* NextGenerator = generator->Append(item);
        Ordinal NewCapacity = capacity.IsInfinite() ? Ordinal::Infinite() : Ordinal(capacity.GetCount() + 1);
        return new LazySequence<ItemType>(NextGenerator, NewCapacity);
    }

    Sequence<ItemType>* InsertAt(const ItemType& item, int TargetIndex) override {
        if (!capacity.IsInfinite() && TargetIndex > capacity.GetCount()) throw std::out_of_range("Индекс выходит за границы");
        if (generator == nullptr) throw std::logic_error("Мутации возможны только на базовых последовательностях");
        Generator<ItemType>* NextGenerator = generator->InsertAt(item, TargetIndex);
        Ordinal NewCapacity = capacity.IsInfinite() ? Ordinal::Infinite() : Ordinal(capacity.GetCount() + 1);
        return new LazySequence<ItemType>(NextGenerator, NewCapacity);
    }

    Sequence<ItemType>* Prepend(const ItemType& item) override { 
        return InsertAt(item, 0); 
    }

    LazySequence<ItemType>* RemoveAt(int TargetIndex) {
        if (!capacity.IsInfinite() && TargetIndex >= capacity.GetCount()) throw std::out_of_range("Индекс выходит за границы");
        if (generator == nullptr) throw std::logic_error("Мутации возможны только на базовых последовательностях");
        Generator<ItemType>* NextGenerator = generator->RemoveAt(TargetIndex);
        Ordinal NewCapacity = capacity.IsInfinite() ? Ordinal::Infinite() : Ordinal(capacity.GetCount() - 1);
        return new LazySequence<ItemType>(NextGenerator, NewCapacity);
    }

    // --- ФУНКЦИОНАЛЬНАЯ АЛГЕБРА ---

    LazySequence<ItemType>* GetSubsequence(int StartIndex, int EndIndex) const {
        auto SubSequenceRule = [this, StartIndex, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
            ItemType RetrievableValue = this->Get(StartIndex + LocalPosition);
            LocalPosition++;
            return RetrievableValue;
        };
        return new LazySequence<ItemType>(SubSequenceRule, Ordinal(EndIndex - StartIndex + 1));
    }

    template <typename ResultType>
    LazySequence<ResultType>* Map(ResultType (*MapperFunction)(const ItemType&)) const {
        auto MapRule = [this, MapperFunction, LocalPosition = 0](Sequence<ResultType>* self) mutable -> ResultType {
            ItemType FetchedValue = this->Get(LocalPosition);
            LocalPosition++;
            return MapperFunction(FetchedValue);
        };
        return new LazySequence<ResultType>(MapRule, this->capacity);
    }

    template <typename ResultType>
    ResultType Reduce(ResultType (*ReduceFunction)(const ResultType&, const ItemType&), ResultType StartValue) const {
        if (IsInfinite()) throw std::logic_error("Reduce невозможен на бесконечной последовательности");
        ResultType AccumulatedResult = StartValue;
        for (int ReduceIndex = 0; ReduceIndex < capacity.GetCount(); ReduceIndex++) {
            AccumulatedResult = ReduceFunction(AccumulatedResult, this->Get(ReduceIndex));
        }
        return AccumulatedResult;
    }

    LazySequence<ItemType>* Where(bool (*PredicateFunction)(const ItemType&)) const {
        auto FilterRule = [this, PredicateFunction, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
            while (true) {
                if (!IsInfinite() && LocalPosition >= capacity.GetCount()) {
                    throw std::out_of_range("Достигнут конец последовательности");
                }
                ItemType FilteredValue = this->Get(LocalPosition);
                LocalPosition++;
                if (PredicateFunction(FilteredValue)) return FilteredValue;
            }
        };
        return new LazySequence<ItemType>(FilterRule, Ordinal::Infinite());
    }

    template <typename OtherItemType>
    LazySequence<std::pair<ItemType, OtherItemType>>* Zip(Sequence<OtherItemType>* OtherSequence) const {
        auto ZipRule = [this, OtherSequence, LocalPosition = 0](Sequence<std::pair<ItemType, OtherItemType>>* self) mutable -> std::pair<ItemType, OtherItemType> {
            ItemType FirstValue = this->Get(LocalPosition);
            OtherItemType SecondValue = OtherSequence->Get(LocalPosition);
            LocalPosition++;
            return std::make_pair(FirstValue, SecondValue);
        };
        return new LazySequence<std::pair<ItemType, OtherItemType>>(ZipRule, this->capacity);
    }
};

#endif // LAZY_SEQUENCE_H