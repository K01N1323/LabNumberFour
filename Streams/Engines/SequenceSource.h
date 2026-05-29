#ifndef SEQUENCE_SOURCE_H
#define SEQUENCE_SOURCE_H

#include <stdexcept>
#include <cstddef> 
#include "Streams/IStreams/IstreamSource.h"
#include "sequences/Sequence.h" 

template <typename ItemType>
class SequenceSource : public IstreamSource<ItemType> {
private:
    Sequence<ItemType>* SourceSequence;
    size_t CurrentPosition;
    bool IsInfiniteSequence;
    size_t KnownLength;
    bool IsClosedFlag; 

public:
    SequenceSource(Sequence<ItemType>* TargetSequence) 
        : SourceSequence(TargetSequence), CurrentPosition(0), IsClosedFlag(false) {
        try {
            KnownLength = static_cast<size_t>(TargetSequence->GetLength());
            IsInfiniteSequence = false;
        } catch (const std::out_of_range&) {
            IsInfiniteSequence = true;
            KnownLength = 0;
        }
    }

    bool IsEndOfStream() const override {
        if (IsClosedFlag) return true; 
        if (IsInfiniteSequence) return false; 
        return CurrentPosition >= KnownLength;
    }

    ItemType Read() override {
        if (IsEndOfStream()) throw std::out_of_range("Достигнут конец потока (или поток закрыт)");
        
        ItemType FetchedValue = SourceSequence->Get(static_cast<int>(CurrentPosition));
        CurrentPosition++;
        return FetchedValue;
    }

    size_t GetPosition() const override { return CurrentPosition; }
    
    bool IsCanSeek() const override { return true; }
    
    size_t Seek(size_t TargetIndex) override {
        if (IsClosedFlag) throw std::logic_error("Невозможно перемещаться по закрытому потоку");
        if (!IsInfiniteSequence && TargetIndex >= KnownLength) {
            throw std::out_of_range("Индекс вне границ последовательности");
        }
        CurrentPosition = TargetIndex;
        return CurrentPosition;
    }

    bool IsCanGoBack() const override { return true; }
    
    void Open() override {
        IsClosedFlag = false; 
    } 
    
    void Close() override {
        IsClosedFlag = true; 
    } 
};

#endif // SEQUENCE_SOURCE_H