#ifndef FILE_TARGET_H
#define FILE_TARGET_H

#include <string>
#include <fstream>
#include <stdexcept>
#include <functional>
#include "../IStreams/IWriteTarget.h"

template <typename ItemType>
class FileTarget : public IWriteTarget<ItemType> {
private:
    std::string TargetFileName;
    std::ofstream OutputFileStream;
    std::function<std::string(const ItemType&)> SerializerFunction;
    size_t CurrentPosition;

public:
    FileTarget(const std::string& FileName, std::function<std::string(const ItemType&)> Serializer)
        : TargetFileName(FileName), SerializerFunction(Serializer), CurrentPosition(0) {}

    void Open() override {
        if (!OutputFileStream.is_open()) {
            OutputFileStream.open(TargetFileName, std::ios::app); 
            if (!OutputFileStream.is_open()) throw std::runtime_error("Не удалось открыть файл для записи");
        }
    }

    void Close() override {
        if (OutputFileStream.is_open()) OutputFileStream.close();
    }

    size_t Write(const ItemType& item) override {
        if (!OutputFileStream.is_open()) throw std::logic_error("Поток не открыт");
        OutputFileStream << SerializerFunction(item) << "\n";
        CurrentPosition++;
        return CurrentPosition;
    }

    size_t GetPosition() const override { return CurrentPosition; }
};

#endif // FILE_TARGET_H