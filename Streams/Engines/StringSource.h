#ifndef STRING_SOURCE_H
#define STRING_SOURCE_H

#include <string>
#include <sstream> 
#include <stdexcept>
#include <functional>
#include "Streams/IStreams/IstreamSource.h"

template <typename ItemType>
class StringSource : public IstreamSource<ItemType> {
private:
    std::string StringData;
    mutable std::stringstream InputStream;
    std::function<ItemType(const std::string&)> DeserializerFunction;
    size_t CurrentPosition;
    bool IsClosedFlag;

public:
    StringSource(const std::string& DataInput, std::function<ItemType(const std::string&)> Deserializer)
        : StringData(DataInput), InputStream(DataInput), DeserializerFunction(Deserializer), CurrentPosition(0), IsClosedFlag(false) {}

    void Open() override {
        IsClosedFlag = false;
    }

    void Close() override {
        IsClosedFlag = true;
    }
    
    bool IsEndOfStream() const override { 
        if (IsClosedFlag) return true; 
        
        InputStream >> std::ws; 
        return InputStream.eof() || (InputStream.peek() == std::char_traits<char>::to_int_type(EOF)); 
    }

    ItemType Read() override {
        if (IsEndOfStream()) throw std::out_of_range("Достигнут конец строки (или поток закрыт)");
        
        std::string ReadWord;
        if (InputStream >> ReadWord) {
            CurrentPosition++;
            return DeserializerFunction(ReadWord);
        }
        throw std::out_of_range("Ошибка чтения строки");
    }

    size_t GetPosition() const override { return CurrentPosition; }
    bool IsCanSeek() const override { return true; }

    size_t Seek(size_t TargetIndex) override {
        if (IsClosedFlag) throw std::logic_error("Невозможно перемещаться по закрытому потоку");
        
        InputStream.clear();
        InputStream.seekg(0, std::ios::beg);
        CurrentPosition = 0;
        
        std::string DummyWord;
        for (size_t SkipIndex = 0; SkipIndex < TargetIndex; SkipIndex++) {
            if (!(InputStream >> DummyWord)) throw std::out_of_range("Индекс выходит за границы");
            CurrentPosition++;
        }
        return CurrentPosition;
    }

    bool IsCanGoBack() const override { return true; }
};

#endif // STRING_SOURCE_H