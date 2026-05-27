#ifndef ALPHABETICAL_INDEX_H
#define ALPHABETICAL_INDEX_H

#include <string>
#include <stdexcept>
#include <cctype> 
#include "sequences/Sequence.h"
#include "sequences/MutableArraySequence.h"
#include "Streams/ReadOnlyStream.h"
#include "LazySequence/LazySequence.h"

struct IndexEntry {
    std::string word;
    MutableArraySequence<size_t>* positions;

    IndexEntry(const std::string& TargetWord) : word(TargetWord) {
        positions = new MutableArraySequence<size_t>();
    }

    IndexEntry(const IndexEntry& other) : word(other.word) {
        positions = new MutableArraySequence<size_t>(*other.positions);
    }

   
    IndexEntry& operator=(const IndexEntry& other) {
        if (this != &other) {
            word = other.word;
            delete positions;
            positions = new MutableArraySequence<size_t>(*other.positions);
        }
        return *this;
    }

    ~IndexEntry() {
        delete positions;
    }
};

class AlphabeticalIndex {
private:
    MutableArraySequence<IndexEntry*>* entries;
    // найти нужную позицию
    int FindInsertPosition(const std::string& TargetWord) const {
        int LeftIndex = 0;
        int RightIndex = entries->GetLength() - 1;

        while (LeftIndex <= RightIndex) {
            int MidIndex = LeftIndex + (RightIndex - LeftIndex) / 2;
            const std::string& MidWord = entries->Get(MidIndex)->word;

            if (MidWord == TargetWord) {
                return MidIndex; 
            }
            if (MidWord < TargetWord) {
                LeftIndex = MidIndex + 1;
            } else {
                RightIndex = MidIndex - 1;
            }
        }
        return LeftIndex; 
    }

    // вставить по нужной позиции
    void ProcessAndInsertWord(const std::string& RawWord, size_t CurrentPosition) {
        std::string CleanWord = "";
        

        for (char ReadCharacter : RawWord) {
            if (std::isalpha(ReadCharacter)) {
                CleanWord += std::tolower(ReadCharacter);
            }
        }

        if (CleanWord.empty()) throw std::logic_error("Переданное выражение не сожержит букв");

        int TargetIndex = FindInsertPosition(CleanWord);

        if (TargetIndex < entries->GetLength() && entries->Get(TargetIndex)->word == CleanWord) {
            entries->Get(TargetIndex)->positions->Append(CurrentPosition);
        } else {
            IndexEntry* NewEntry = new IndexEntry(CleanWord);
            NewEntry->positions->Append(CurrentPosition);
            
            if (TargetIndex >= entries->GetLength()) {
                entries->Append(NewEntry);
            } else {
                entries->InsertAt(NewEntry, TargetIndex);
            }
        }
    }

public:
    AlphabeticalIndex() {
        entries = new MutableArraySequence<IndexEntry*>();
    }

    ~AlphabeticalIndex() {
        for (int ElementIndex = 0; ElementIndex < entries->GetLength(); ElementIndex++) {
            delete entries->Get(ElementIndex);
        }
        delete entries;
    }

    // строим по потоку 
    void BuildFromStream(ReadOnlyStream<std::string>* InputStream) {
        while (!InputStream->IsEndOfStream()) {
            std::string ReadWord;
            try {
                ReadWord = InputStream->Read();
            } catch (const std::out_of_range&) {
                break; 
            }
            size_t CurrentPosition = InputStream->GetPosition();
            ProcessAndInsertWord(ReadWord, CurrentPosition);
        }
    }
    // строим по lazyseq
    void BuildFromLazySequence(LazySequence<std::string>* InputSequence) {
        if (InputSequence->IsInfinite()) {
            throw std::logic_error("Невозможно построить полный индекс для бесконечной последовательности");
        }
        
        int SequenceLength = InputSequence->GetLength();
        for (int ElementIndex = 0; ElementIndex < SequenceLength; ElementIndex++) {
            std::string ReadWord = InputSequence->Get(ElementIndex);
     
            ProcessAndInsertWord(ReadWord, static_cast<size_t>(ElementIndex)); 
        }
    }
    // уник. слова
    int GetUniqueWordsCount() const {
        return entries->GetLength();
    }
    // конкретное слово 
    const IndexEntry* GetEntryAt(int TargetIndex) const {
        if (TargetIndex < 0 || TargetIndex >= entries->GetLength()) {
            throw std::out_of_range("Индекс выходит за границы");
        }
        return entries->Get(TargetIndex);
    }
};

#endif // ALPHABETICAL_INDEX_H