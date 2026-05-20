#ifndef WRITE_ONLY_STREAM_H
#define WRITE_ONLY_STREAM_H

#include <string>
#include <functional>
#include "IStreams/IWriteTarget.h"
#include "Engines/FileTarget.h"
#include "Engines/SequenceTarget.h"

template <typename ItemType>
class WriteOnlyStream {
private:
    IWriteTarget<ItemType>* StreamTarget;
    bool OwnsMemoryFlag;

public:
    WriteOnlyStream(const char* FileName, std::function<std::string(const ItemType&)> SerializerFunction) : OwnsMemoryFlag(true) {
        this->StreamTarget = new FileTarget<ItemType>(std::string(FileName), SerializerFunction);
    }

    WriteOnlyStream(Sequence<ItemType>* TargetSequence) : OwnsMemoryFlag(true) {
        this->StreamTarget = new SequenceTarget<ItemType>(TargetSequence);
    }

    WriteOnlyStream(WriteOnlyStream<ItemType>* OtherStream) : OwnsMemoryFlag(false) {
        this->StreamTarget = OtherStream->StreamTarget;
    }

    ~WriteOnlyStream() {
        if (OwnsMemoryFlag) {
            delete StreamTarget;
        }
    }

    size_t GetPosition() const { return StreamTarget->GetPosition(); }
    size_t Write(const ItemType& item) { return StreamTarget->Write(item); }
    void Open() { StreamTarget->Open(); }
    void Close() { StreamTarget->Close(); }
};

#endif // WRITE_ONLY_STREAM_H