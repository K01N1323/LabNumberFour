#ifndef FILE_SOURCE_H
#define FILE_SOURCE_H

#include <string>
#include <fstream>
#include <stdexcept>
#include <functional>
#include "Streams/IStreams/IstreamSource.h"

template <typename ItemType>
class FileSource : public IstreamSource<ItemType> {
private:
    std::string TargetFileName;
    std::ifstream InputFileStream;
    std::function<ItemType(const std::string&)> DeserializerFunction;
    size_t CurrentPosition;

public:
    FileSource(const std::string& FileName, std::function<ItemType(const std::string&)> Deserializer)
        : TargetFileName(FileName), DeserializerFunction(Deserializer), CurrentPosition(0) {}

    void Open() override {
        if (!InputFileStream.is_open()) {
            InputFileStream.open(TargetFileName);
            if (!InputFileStream.is_open()) throw std::runtime_error("Не удалось открыть файл для чтения");
        }
    }

    void Close() override {
        if (InputFileStream.is_open()) InputFileStream.close();
    }

    bool IsEndOfStream() const override { return InputFileStream.eof(); }

    ItemType Read() override {
        if (!InputFileStream.is_open()) throw std::logic_error("Поток не открыт");
        if (IsEndOfStream()) throw std::out_of_range("Достигнут конец файла");

        std::string ReadLine;
        if (std::getline(InputFileStream, ReadLine)) {
            CurrentPosition++;
            return DeserializerFunction(ReadLine); 
        }
        throw std::out_of_range("Ошибка чтения");
    }

    size_t GetPosition() const override { return CurrentPosition; }
    
    bool IsCanSeek() const override { return true; }

    size_t Seek(size_t TargetIndex) override {
        if (!InputFileStream.is_open()) throw std::logic_error("Поток не открыт");
        InputFileStream.clear();
        InputFileStream.seekg(0, std::ios::beg);
        CurrentPosition = 0;
        
        std::string DummyLine;
        for (size_t SkipIndex = 0; SkipIndex < TargetIndex; SkipIndex++) {
            if (!std::getline(InputFileStream, DummyLine)) throw std::out_of_range("Индекс выходит за границы");
            CurrentPosition++;
        }
        return CurrentPosition;
    }

    bool IsCanGoBack() const override { return true; }
};

#endif // FILE_SOURCE_H