#ifndef FILE_SOURCE_H
#define FILE_SOURCE_H

#include <string>
#include <fstream>
#include <stdexcept>
#include <functional>
#include "../IStreams/IstreamSource.h"

template <typename ItemType>
class FileSource : public IstreamSource<ItemType> {
private:
    std::string TargetFileName;
    mutable std::ifstream InputFileStream;
    std::function<ItemType(const std::string&)> DeserializerFunction;
    size_t CurrentPosition;

public:
    FileSource(const std::string& FileName, std::function<ItemType(const std::string&)> Deserializer): TargetFileName(FileName), DeserializerFunction(Deserializer), CurrentPosition(0) {}

    void Open() override {
        if (!InputFileStream.is_open()) {
            InputFileStream.open(TargetFileName, std::ios::in);
            if (!InputFileStream.is_open()) throw std::runtime_error("Не удалось открыть файл для чтения:" + TargetFileName);
        }
    }

    void Close() override {
        if (InputFileStream.is_open()) InputFileStream.close();
    }

    bool IsEndOfStream() const override { 
        if (!InputFileStream.is_open()) return true;
        InputFileStream >> std::ws; // Пропускаем пробельные символы
        return InputFileStream.eof() || (InputFileStream.peek() == std::char_traits<char>::to_int_type(EOF)); 
    }

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
        
        // за о(1) тк seekg делает смещение в байтах
        InputFileStream.seekg(TargetIndex, std::ios::beg);
        if (InputFileStream.fail()) throw std::out_of_range("Смещение выходит за границы файла");
        
        CurrentPosition = TargetIndex;
        return CurrentPosition;
    }

    bool IsCanGoBack() const override { return true; }
};

#endif // FILE_SOURCE_H