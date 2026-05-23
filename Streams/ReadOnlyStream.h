#ifndef READ_ONLY_STREAM_H
#define READ_ONLY_STREAM_H

#include <string>
#include <functional>
#include "Streams/IStreams/IstreamSource.h"
#include "Streams/Engines/SequenceSource.h"
#include "Streams/Engines/FileSource.h"
#include "Streams/Engines/StringSource.h"

template <typename ItemType>
class ReadOnlyStream {
private:
    IstreamSource<ItemType>* StreamSource;
    bool OwnsMemoryFlag; 

public:
    ReadOnlyStream(Sequence<ItemType>* TargetSequence) : OwnsMemoryFlag(true) {
        this->StreamSource = new SequenceSource<ItemType>(TargetSequence);
    }

    ReadOnlyStream(const char* FileName, std::function<ItemType(const std::string&)> DeserializerFunction) : OwnsMemoryFlag(true) {
        this->StreamSource = new FileSource<ItemType>(std::string(FileName), DeserializerFunction);
    }

    ReadOnlyStream(const std::string& StringData, std::function<ItemType(const std::string&)> DeserializerFunction) : OwnsMemoryFlag(true) {
        this->StreamSource = new StringSource<ItemType>(StringData, DeserializerFunction);
    }

    ReadOnlyStream(ReadOnlyStream<ItemType>* OtherStream) : OwnsMemoryFlag(false) {
        this->StreamSource = OtherStream->StreamSource;
    }

    ~ReadOnlyStream() {
        if (OwnsMemoryFlag) {
            delete StreamSource;
        }
    }
    
    bool IsEndOfStream() const { return StreamSource->IsEndOfStream(); }
    ItemType Read() { return StreamSource->Read(); }
    size_t GetPosition() const { return StreamSource->GetPosition(); }
    bool IsCanSeek() const { return StreamSource->IsCanSeek(); }
    size_t Seek(size_t TargetIndex) { return StreamSource->Seek(TargetIndex); }
    bool IsCanGoBack() const { return StreamSource->IsCanGoBack(); }
    void Open() { StreamSource->Open(); }
    void Close() { StreamSource->Close(); }
};

#endif // READ_ONLY_STREAM_H