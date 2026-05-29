#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include <stdexcept>
#include <functional>
#include "sequences/Sequence.h"
#include "sequences/MutableArraySequence.h"
#include "Generators/Generator.h"
#include "Ordinal/Ordinal.h"

template <typename FirstType, typename SecondType>
struct Pair {
    FirstType first;  
    SecondType second; 

    Pair() : first(FirstType()), second(SecondType()) {}

    Pair(const FirstType& FirstValue, const SecondType& SecondValue) : first(FirstValue), second(SecondValue) {}

    bool operator==(const Pair& other) const {
        return first == other.first && second == other.second;
    }
};

template <typename ItemType> 
class LazySequence : public Sequence<ItemType> {
private:
    Generator<ItemType>* generator;
    MutableArraySequence<ItemType>* cache;
    Ordinal capacity; 
    
    MutableArraySequence<LazySequence<ItemType>*>* ChunkList; 

    // для конката
    LazySequence(MutableArraySequence<LazySequence<ItemType>*>* CombinedChunks) : generator(nullptr), cache(nullptr), ChunkList(CombinedChunks) {
        int TotalOmega = 0;
        int TotalIndex = 0;
        for (int ChunkIndex = 0; ChunkIndex < ChunkList->GetLength(); ChunkIndex++) {
            Ordinal ChunkCapacity = ChunkList->Get(ChunkIndex)->GetLengthOrdinal();
            TotalOmega += ChunkCapacity.omega;
            TotalIndex += ChunkCapacity.index;
        }
        this->capacity = Ordinal(TotalOmega, TotalIndex);
    }
    // для GetSubSequence
    LazySequence(Generator<ItemType>* ExistingGenerator, Ordinal NewCapacity) : generator(ExistingGenerator), cache(new MutableArraySequence<ItemType>()), capacity(NewCapacity) {
        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
        if (this->generator != nullptr) {
            this->generator->SetOwner(this);
        }
    }

    // всомагательный метод для гет из сконкаченных последовательностей
    const ItemType& GetFromComposite(Ordinal TargetOrdinal) const {
        int TargetInfinities = TargetOrdinal.omega;
        int RemainingIndex = TargetOrdinal.index;

        for (int index = 0; index < ChunkList->GetLength(); index++) {
            LazySequence<ItemType>* CurrentChunk = ChunkList->Get(index);

            if (TargetInfinities == 0) {
                if (CurrentChunk->IsInfinite()) {
                    return CurrentChunk->Get(Ordinal(0, RemainingIndex));
                } else {
                    int ChunkLength = CurrentChunk->GetLength();
                    if (RemainingIndex < ChunkLength) {
                        return CurrentChunk->Get(Ordinal(0, RemainingIndex));
                    } else {
                        RemainingIndex -= ChunkLength;
                    }
                }
            } else {
                if (CurrentChunk->IsInfinite()) {
                    TargetInfinities--;
                }
            }
        }
        throw std::out_of_range("Ординал выходит за границы склеенной последовательности");
    }

    // вспомагательный метод для get из базовой последовательности 
    const ItemType& GetFromBase(Ordinal TargetOrdinal) const {
        if (TargetOrdinal.omega > 0) throw std::logic_error("Дельта больше нуля недопустима для одиночного чанка");
        
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

public:
    LazySequence(std::function<ItemType(Sequence<ItemType>*)> RuleFunction, Ordinal InitialCapacity) : cache(new MutableArraySequence<ItemType>()), capacity(InitialCapacity) {
        this->generator = new Generator<ItemType>(this, RuleFunction);
        
        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
    }

   LazySequence() : cache(new MutableArraySequence<ItemType>()), capacity(Ordinal(0, 0)) {
        auto EmptyRule = [](Sequence<ItemType>* self) -> ItemType {
            throw std::out_of_range("Для данной последовательности еще нет генератора");
        };
        this->generator = new Generator<ItemType>(this, EmptyRule);
        
        this->ChunkList = new MutableArraySequence<LazySequence<ItemType>*>();
        this->ChunkList->Append(this);
    }

    LazySequence(ItemType* items, int count) : cache(new MutableArraySequence<ItemType>()), capacity(Ordinal(0, count)) {
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

    LazySequence(const LazySequence<ItemType>& other) : capacity(other.capacity) {
        this->cache = new MutableArraySequence<ItemType>();
        if (other.cache) {
            for (int CacheIndex = 0; CacheIndex < other.cache->GetLength(); CacheIndex++) {
                this->cache->Append(other.cache->Get(CacheIndex));
            }
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
        if (cache != nullptr) {
            return cache->GetLength();
        } else {
            return 0;
        }
    }

    // геттеры

    const ItemType& Get(Ordinal TargetOrdinal) const {
        if (generator == nullptr && cache == nullptr) {
            return GetFromComposite(TargetOrdinal);
        }
       
        return GetFromBase(TargetOrdinal);
    }
       

    const ItemType& Get(int TargetIndex) const override {
        return Get(Ordinal(0, TargetIndex)); 
    }

    const ItemType& GetFirst() const override { return this->Get(Ordinal(0, 0)); }

    const ItemType& GetLast() const override {
        if (IsInfinite()) throw std::logic_error("Нельзя получить последний элемент бесконечной последовательности");
        if (generator == nullptr && cache == nullptr) { 
            LazySequence<ItemType>* LastChunk = ChunkList->Get(ChunkList->GetLength() - 1);
            return LastChunk->Get(Ordinal(0, LastChunk->GetLengthOrdinal().GetCount() - 1));
        }
        if (capacity.GetCount() == 0) throw std::out_of_range("Последовательность пуста");
        return this->Get(Ordinal(0, capacity.GetCount() - 1));
    }

    Sequence<ItemType>* Instance() override { return this; }

    Sequence<ItemType>* CreateEmpty() const override {
        return new LazySequence<ItemType>();
    }

    IEnumerator<ItemType>* GetEnumerator() const override {
        class LazyEnumerator : public IEnumerator<ItemType> {
        private:
            const LazySequence<ItemType>* TargetSequence;
            Ordinal CurrentPosition;
        public:
            LazyEnumerator(const LazySequence<ItemType>* SequencePointer) : TargetSequence(SequencePointer), CurrentPosition(0, 0) {}

            bool HasNext() const override {
                if (TargetSequence->IsInfinite()) return true;
                int FlatIndex = CurrentPosition.index; 
                return FlatIndex < TargetSequence->GetLength();
            }

            void MoveNext() override { 
                CurrentPosition.index++; 
            }

            const ItemType& GetCurrent() const override { 
                return TargetSequence->Get(CurrentPosition); 
            }
        };
        return new LazyEnumerator(this);
    }
  
    // 1. Диспетчер
    Sequence<ItemType>* Concat(Sequence<ItemType>* list) override {
        LazySequence<ItemType>* OtherLazy = dynamic_cast<LazySequence<ItemType>*>(list);
        if (OtherLazy) {
            return this->ConcatLazy(OtherLazy);
        }
    
        return this->ConcatDefault(list);
    }


    Sequence<ItemType>* ConcatDefault(Sequence<ItemType>* list) const {
        auto ConcatRule = [this, list, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
            if (!this->IsInfinite() && LocalPosition >= this->capacity.GetCount()) {
                int OffsetIndex = this->capacity.GetCount();
                ItemType FetchedValue = list->Get(LocalPosition - OffsetIndex);
                LocalPosition++;
                return FetchedValue;
            } else {
                ItemType FetchedValue = this->Get(LocalPosition);
                LocalPosition++;
                return FetchedValue;
            }
        };
        
        Ordinal NewCapacity = this->IsInfinite() ? Ordinal::Infinite() : Ordinal(this->capacity.GetCount() + list->GetLength());
        return new LazySequence<ItemType>(ConcatRule, NewCapacity);
    }

    
    LazySequence<ItemType>* ConcatLazy(LazySequence<ItemType>* OtherSequence) const {
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
        if (IsInfinite()) throw std::logic_error("Нельзя добавить элемент в конец бесконечной последовательности");

        return InsertAt(item, this->GetLength());
    }

    Sequence<ItemType>* InsertAt(const ItemType& item, int TargetIndex) override {
        if (!capacity.IsInfinite() && TargetIndex > capacity.GetCount()) throw std::out_of_range("Индекс выходит за границы");

        if (generator != nullptr) {
            Generator<ItemType>* NextGenerator = generator->InsertAt(item, TargetIndex);
            Ordinal NewCapacity = capacity.IsInfinite() ? Ordinal::Infinite() : Ordinal(capacity.GetCount() + 1);
            return new LazySequence<ItemType>(NextGenerator, NewCapacity);
        } 
        else {
            ItemType SingleArray[1] = {item};
            LazySequence<ItemType>* SingleElementSeq = new LazySequence<ItemType>(SingleArray, 1);
            return this->InsertSequenceAt(SingleElementSeq, TargetIndex);
        }
    }

    LazySequence<ItemType>* InsertSequenceAt(LazySequence<ItemType>* InsertedSeq, int TargetIndex) const {
        if (!capacity.IsInfinite() && TargetIndex > capacity.GetCount()) {
            throw std::out_of_range("Индекс выходит за границы исходной последовательности");
        }

        auto HeadRule = [this, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
            ItemType FetchedValue = this->Get(LocalPosition);
            LocalPosition++;
            return FetchedValue;
        };
        LazySequence<ItemType>* HeadSeq = new LazySequence<ItemType>(HeadRule, Ordinal(TargetIndex));

        auto TailRule = [this, TargetIndex, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
            ItemType FetchedValue = this->Get(TargetIndex + LocalPosition);
            LocalPosition++;
            return FetchedValue;
        };
        Ordinal TailCapacity = this->IsInfinite() ? Ordinal::Infinite() : Ordinal(this->capacity.GetCount() - TargetIndex);
        LazySequence<ItemType>* TailSeq = new LazySequence<ItemType>(TailRule, TailCapacity);

        LazySequence<ItemType>* Step1 = HeadSeq->ConcatLazy(InsertedSeq);
        return Step1->ConcatLazy(TailSeq);
    }

    Sequence<ItemType>* Prepend(const ItemType& item) override { 
        return InsertAt(item, 0); 
    }

    
    LazySequence<ItemType>* RemoveAt(int TargetIndex) {
        if (!capacity.IsInfinite() && TargetIndex >= capacity.GetCount()) throw std::out_of_range("Индекс выходит за границы");

        if (generator != nullptr) {
            Generator<ItemType>* NextGenerator = generator->RemoveAt(TargetIndex);
            Ordinal NewCapacity = capacity.IsInfinite() ? Ordinal::Infinite() : Ordinal(capacity.GetCount() - 1);
            return new LazySequence<ItemType>(NextGenerator, NewCapacity);
        } 
        else {
            auto HeadRule = [this, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
                ItemType FetchedValue = this->Get(LocalPosition);
                LocalPosition++;
                return FetchedValue;
            };
            LazySequence<ItemType>* HeadSeq = new LazySequence<ItemType>(HeadRule, Ordinal(TargetIndex));

            auto TailRule = [this, TargetIndex, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
                ItemType FetchedValue = this->Get(TargetIndex + 1 + LocalPosition);
                LocalPosition++;
                return FetchedValue;
            };
            Ordinal TailCapacity = this->IsInfinite() ? Ordinal::Infinite() : Ordinal(this->capacity.GetCount() - TargetIndex - 1);
            LazySequence<ItemType>* TailSeq = new LazySequence<ItemType>(TailRule, TailCapacity);

            return HeadSeq->ConcatLazy(TailSeq);
        }
    }

    // функ. алгебра 
    Sequence<ItemType>* GetSubsequence(int StartIndex, int EndIndex) const override {
        auto SubSequenceRule = [this, StartIndex, LocalPosition = 0](Sequence<ItemType>* self) mutable -> ItemType {
            ItemType RetrievableValue = this->Get(StartIndex + LocalPosition);
            LocalPosition++;
            return RetrievableValue;
        };
        return new LazySequence<ItemType>(SubSequenceRule, Ordinal(EndIndex - StartIndex + 1));
    }

    template <typename ResultType, typename MapperFunc>
    LazySequence<ResultType>* Map(MapperFunc MapperFunction) const {
        auto MapRule = [this, MapperFunction, LocalPosition = 0](Sequence<ResultType>* self) mutable -> ResultType {
            ItemType FetchedValue = this->Get(LocalPosition);
            LocalPosition++;
            return MapperFunction(FetchedValue);
        };
        return new LazySequence<ResultType>(MapRule, this->capacity);
    }

    template <typename ReducerFunc, typename ResultType>
    ResultType Reduce(ReducerFunc ReduceFunction, ResultType StartValue) const {
        if (IsInfinite()) throw std::logic_error("Reduce невозможна на бесконечной последовательности");
        ResultType AccumulatedResult = StartValue;
        for (int ReduceIndex = 0; ReduceIndex < capacity.GetCount(); ReduceIndex++) {
            AccumulatedResult = ReduceFunction(AccumulatedResult, this->Get(ReduceIndex));
        }
        return AccumulatedResult;
    }

    template <typename PredicateFunc>
    LazySequence<ItemType>* Where(PredicateFunc PredicateFunction) const {
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
    LazySequence<Pair<ItemType, OtherItemType>>* Zip(Sequence<OtherItemType>* OtherSequence) const {
        auto ZipRule = [this, OtherSequence, LocalPosition = 0](Sequence<Pair<ItemType, OtherItemType>>* self) mutable -> Pair<ItemType, OtherItemType> {
            ItemType FirstValue = this->Get(LocalPosition);
            OtherItemType SecondValue = OtherSequence->Get(LocalPosition);
            LocalPosition++;
            return Pair<ItemType, OtherItemType>(FirstValue, SecondValue);
        };
        return new LazySequence<Pair<ItemType, OtherItemType>>(ZipRule, this->capacity);
    }

  
  
};

#endif // LAZY_SEQUENCE_H