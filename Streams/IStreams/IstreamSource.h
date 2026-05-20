#ifndef ISTREAM_SOURCE_H
#define ISTREAM_SOURCE_H

#include <cstddef>

template <typename ItemType>
class IstreamSource {
public:
    virtual bool IsEndOfStream() const = 0;
    virtual ItemType Read() = 0; // ИСПРАВЛЕНО: возврат по значению
    virtual size_t GetPosition() const = 0;
    virtual bool IsCanSeek() const = 0;
    virtual size_t Seek(size_t TargetIndex) = 0;
    virtual bool IsCanGoBack() const = 0;
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual ~IstreamSource() = default;
};

#endif // ISTREAM_SOURCE_H