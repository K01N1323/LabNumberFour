#ifndef SEQUENCE_TARGET_H
#define SEQUENCE_TARGET_H

#include <stdexcept>
#include "Streams/IStreams/IWriteTarget.h"
#include "sequences/Sequence.h"

template <typename ItemType>
class SequenceTarget : public IWriteTarget<ItemType> {
private:
    Sequence<ItemType>* TargetSequence;
    size_t CurrentPosition;
    bool IsClosedFlag; 

public:
    SequenceTarget(Sequence<ItemType>* SequenceTargetObject) : TargetSequence(SequenceTargetObject), IsClosedFlag(false) {
        try {
            CurrentPosition = static_cast<size_t>(TargetSequence->GetLength());
        } catch (const std::out_of_range&) {
            CurrentPosition = 0; 
        }
    }

    void Open() override {
        IsClosedFlag = false;
    }

    void Close() override {
        IsClosedFlag = true; 
    }

    size_t Write(const ItemType& item) override {
        if (IsClosedFlag) throw std::logic_error("Попытка записи в закрытый поток");
        
        TargetSequence->Append(item);
        CurrentPosition++;
        return CurrentPosition;
    }

    size_t GetPosition() const override { return CurrentPosition; }
};

#endif //  SEQUENCE_TARGET_H