#ifndef I_WRITE_TARGET_H
#define I_WRITE_TARGET_H

#include <cstddef>

template <typename ItemType>
class IWriteTarget {
public:
    virtual size_t Write(const ItemType& item) = 0; 
    virtual size_t GetPosition() const = 0;
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual ~IWriteTarget() = default;
};

#endif // I_WRITE_TARGET_H