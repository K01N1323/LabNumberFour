#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <string>
#include <limits>
#include <utility>
#include <fstream>
#include "LazySequence/LazySequence.h"
#include "Generators/GeneratorRules.h"
#include "Streams/ReadOnlyStream.h"
#include "Streams/WriteOnlyStream.h"
#include "Tasks/AlphabeticalIndex.h"

class ConsoleInterface {
private:
    LazySequence<int>* ActiveIntegerSequence;
    AlphabeticalIndex* ActiveIndex;

    void ClearInputBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void PrintSeparator() const {
        std::cout << "\n==================================================\n";
    }

    // --- ПОДМЕНЮ 1: Ленивые последовательности ---
    void HandleLazySequences() {
        int SequenceChoice = -1;
        while (SequenceChoice != 0) {
            PrintSeparator();
            std::cout << "[ ЛЕНИВЫЕ ПОСЛЕДОВАТЕЛЬНОСТИ И АЛГЕБРА ]\n";
            std::cout << "1. Создать Арифметическую прогрессию\n";
            std::cout << "2. Создать Фибоначчи\n";
            std::cout << "3. Проверить Map (Умножить элементы на 10)\n";
            std::cout << "4. Проверить Where (Оставить только четные)\n";
            std::cout << "5. Проверить Reduce (Сумма первых N элементов)\n";
            std::cout << "6. Проверить Zip (Сцепить с другой прогрессией)\n";
            std::cout << "7. Проверить GetSubsequence (Получить подсписок)\n";
            std::cout << "8. Вывести элементы и проверить материализацию\n";
            std::cout << "9. Проверить Concat (Конкатенация и доступ по ординалам)\n";
            std::cout << "0. Назад\n";
            std::cout << "Ваш выбор: ";
            std::cin >> SequenceChoice;

            if (std::cin.fail()) { ClearInputBuffer(); continue; }

            try {
                switch (SequenceChoice) {
                    case 1: {
                        delete ActiveIntegerSequence;
                        ActiveIntegerSequence = new LazySequence<int>(CreateArithmeticProgression(1, 2), Ordinal::Infinite());
                        std::cout << "Создана прогрессия: 1, 3, 5, 7...\n";
                        break;
                    }
                    case 2: {
                        delete ActiveIntegerSequence;
                        ActiveIntegerSequence = new LazySequence<int>(CreateFibonacciSequence(), Ordinal::Infinite());
                        std::cout << "Создана последовательность Фибоначчи: 0, 1, 1, 2, 3, 5...\n";
                        break;
                    }
                    case 3: {
                        if (!ActiveIntegerSequence) { std::cout << "Сначала создайте последовательность!\n"; break; }
                        auto MultiplyByTen = [](const int& value) -> int { return value * 10; };
                        LazySequence<int>* MappedSequence = ActiveIntegerSequence->Map(MultiplyByTen);
                        std::cout << "Первые 5 элементов после Map: ";
                        for (int PrintIndex = 0; PrintIndex < 5; PrintIndex++) {
                            std::cout << MappedSequence->Get(PrintIndex) << " ";
                        }
                        std::cout << "\n";
                        delete MappedSequence;
                        break;
                    }
                    case 4: {
                        if (!ActiveIntegerSequence) { std::cout << "Сначала создайте последовательность!\n"; break; }
                        auto IsEven = [](const int& value) -> bool { return value % 2 == 0; };
                        LazySequence<int>* FilteredSequence = ActiveIntegerSequence->Where(IsEven);
                        std::cout << "Первые 5 четных элементов (Генерация Where): ";
                        for (int PrintIndex = 0; PrintIndex < 5; PrintIndex++) {
                            std::cout << FilteredSequence->Get(PrintIndex) << " ";
                        }
                        std::cout << "\n";
                        delete FilteredSequence;
                        break;
                    }
                    case 5: {
                        if (!ActiveIntegerSequence) { std::cout << "Сначала создайте последовательность!\n"; break; }
                        int ElementCount;
                        std::cout << "Сколько первых элементов просуммировать (Reduce)? ";
                        std::cin >> ElementCount;
                        
                        LazySequence<int>* SubSequence = ActiveIntegerSequence->GetSubsequence(0, ElementCount - 1);
                        auto SumReducer = [](const int& AccumulatedValue, const int& CurrentValue) -> int {
                            return AccumulatedValue + CurrentValue;
                        };
                        
                        int TotalSum = SubSequence->Reduce(SumReducer, 0);
                        std::cout << "Результат Reduce (Сумма): " << TotalSum << "\n";
                        delete SubSequence;
                        break;
                    }
                    case 6: {
                        if (!ActiveIntegerSequence) { std::cout << "Сначала создайте последовательность!\n"; break; }
                        LazySequence<int> SecondSequence(CreateArithmeticProgression(10, 10), Ordinal::Infinite());
                        LazySequence<std::pair<int, int>>* ZippedSequence = ActiveIntegerSequence->Zip(&SecondSequence);
                        
                        std::cout << "Первые 5 пар (Zip с прогрессией шагом 10):\n";
                        for (int PrintIndex = 0; PrintIndex < 5; PrintIndex++) {
                            std::pair<int, int> CurrentPair = ZippedSequence->Get(PrintIndex);
                            std::cout << "[" << CurrentPair.first << ", " << CurrentPair.second << "] ";
                        }
                        std::cout << "\n";
                        delete ZippedSequence;
                        break;
                    }
                    case 7: {
                        if (!ActiveIntegerSequence) { std::cout << "Сначала создайте последовательность!\n"; break; }
                        int StartIndex, EndIndex;
                        std::cout << "Начальный индекс: "; std::cin >> StartIndex;
                        std::cout << "Конечный индекс: "; std::cin >> EndIndex;
                        
                        LazySequence<int>* SubSequence = ActiveIntegerSequence->GetSubsequence(StartIndex, EndIndex);
                        std::cout << "Подсписок: ";
                        for (int PrintIndex = 0; PrintIndex < SubSequence->GetLength(); PrintIndex++) {
                            std::cout << SubSequence->Get(PrintIndex) << " ";
                        }
                        std::cout << "\n";
                        delete SubSequence;
                        break;
                    }
                    case 8: {
                        if (!ActiveIntegerSequence) { std::cout << "Сначала создайте последовательность!\n"; break; }
                        int PrintCount;
                        std::cout << "Сколько элементов вывести? "; std::cin >> PrintCount;
                        for (int PrintIndex = 0; PrintIndex < PrintCount; PrintIndex++) {
                            std::cout << ActiveIntegerSequence->Get(PrintIndex) << " ";
                        }
                        std::cout << "\nМатериализовано в кэше: " << ActiveIntegerSequence->GetMaterializedCount() << "\n";
                        break;
                    }
                    case 9: { // ТЕСТ КОНКАТЕНАЦИИ
                        if (!ActiveIntegerSequence) { std::cout << "Сначала создайте первую последовательность!\n"; break; }
                        
                        std::cout << "Первая последовательность (Чанк 0) - активная.\n";

                        std::cout << "Создаем вторую последовательность (Чанк 1: числа 99, 88, 77)...\n";
                        int ArrayValues[] = {99, 88, 77};
                        LazySequence<int> SecondSequence(ArrayValues, 3);

                        std::cout << "Выполняем Concat...\n";
                        LazySequence<int>* ConcatenatedSequence = ActiveIntegerSequence->Concat(&SecondSequence);

                        int ChunkDelta, LocalIndex;
                        std::cout << "Введите дельту (номер чанка, например 0 или 1): ";
                        std::cin >> ChunkDelta;
                        std::cout << "Введите локальный индекс в чанке: ";
                        std::cin >> LocalIndex;

                        try {
                            int ResultValue = ConcatenatedSequence->Get(Ordinal(ChunkDelta, LocalIndex));
                            std::cout << "Результат по ординалу (" << ChunkDelta << ", " << LocalIndex << "): " << ResultValue << "\n";
                        } catch (const std::exception& ErrorMessage) {
                            std::cout << "ОШИБКА ДОСТУПА ПО ОРДИНАЛУ: " << ErrorMessage.what() << "\n";
                        }
                        
                        delete ConcatenatedSequence;
                        break;
                    }
                    case 0: break;
                    default: std::cout << "Неизвестная команда.\n";
                }
            } catch (const std::exception& ErrorMessage) {
                std::cout << "ОШИБКА: " << ErrorMessage.what() << "\n";
            }
        }
    }

    // --- ПОДМЕНЮ 2: Тестирование потоков (Streams) ---
    void HandleStreams() {
        int StreamChoice = -1;
        while (StreamChoice != 0) {
            PrintSeparator();
            std::cout << "[ ТЕСТИРОВАНИЕ ПОТОКОВ (READ / WRITE STREAMS) ]\n";
            std::cout << "1. Записать элементы в файл (FileTarget)\n";
            std::cout << "2. Прочитать элементы из файла (FileSource)\n";
            std::cout << "3. Прочитать строку (StringSource)\n";
            std::cout << "4. Прочитать из ленивой последовательности (ReadOnlyStream поверх Sequence)\n";
            std::cout << "5. Записать в динамический массив (WriteOnlyStream поверх Sequence)\n";
            std::cout << "0. Назад\n";
            std::cout << "Ваш выбор: ";
            std::cin >> StreamChoice;

            if (std::cin.fail()) { ClearInputBuffer(); continue; }

            try {
                switch (StreamChoice) {
                    case 1: {
                        std::string OutputFileName = "test_stream_output.txt";
                        auto IntegerSerializer = [](const int& value) -> std::string { return std::to_string(value); };
                        
                        WriteOnlyStream<int> FileTargetStream(OutputFileName.c_str(), IntegerSerializer);
                        FileTargetStream.Open();
                        
                        std::cout << "Пишем числа 100, 200, 300 в файл...\n";
                        FileTargetStream.Write(100);
                        FileTargetStream.Write(200);
                        FileTargetStream.Write(300);
                        
                        FileTargetStream.Close();
                        std::cout << "Успешно записано в " << OutputFileName << "\n";
                        break;
                    }
                    case 2: {
                        std::string InputFileName = "test_stream_output.txt";
                        auto IntegerDeserializer = [](const std::string& text) -> int { return std::stoi(text); };
                        
                        ReadOnlyStream<int> FileSourceStream(InputFileName.c_str(), IntegerDeserializer);
                        FileSourceStream.Open();
                        
                        std::cout << "Читаем из файла " << InputFileName << ":\n";
                        while (!FileSourceStream.IsEndOfStream()) {
                            try {
                                int ReadValue = FileSourceStream.Read();
                                std::cout << "Прочитано: " << ReadValue << " (Позиция: " << FileSourceStream.GetPosition() << ")\n";
                            } catch (const std::out_of_range&) {
                                break;
                            }
                        }
                        FileSourceStream.Close();
                        break;
                    }
                    case 3: {
                        ClearInputBuffer();
                        std::string InputText;
                        std::cout << "Введите строку чисел через пробел: ";
                        std::getline(std::cin, InputText);
                        
                        auto IntegerDeserializer = [](const std::string& text) -> int { return std::stoi(text); };
                        ReadOnlyStream<int> StringSourceStream(InputText, IntegerDeserializer);
                        
                        std::cout << "Читаем через StringSource:\n";
                        while (!StringSourceStream.IsEndOfStream()) {
                            try {
                                int ReadValue = StringSourceStream.Read();
                                std::cout << "Значение: " << ReadValue << "\n";
                            } catch (const std::out_of_range&) {
                                break;
                            }
                        }
                        break;
                    }
                    case 4: { 
                        if (!ActiveIntegerSequence) { std::cout << "Создайте последовательность в первом меню!\n"; break; }
                        
                        std::cout << "Оборачиваем текущую ленивую последовательность в ReadOnlyStream...\n";
                        ReadOnlyStream<int> SequenceReadStream(ActiveIntegerSequence);
                        SequenceReadStream.Open();
                        
                        int ElementsToRead;
                        std::cout << "Сколько элементов прочитать из потока? ";
                        std::cin >> ElementsToRead;

                        for (int ReadIndex = 0; ReadIndex < ElementsToRead; ReadIndex++) {
                            if (SequenceReadStream.IsEndOfStream()) break;
                            std::cout << "Поток прочитал: " << SequenceReadStream.Read() 
                                      << " (Позиция каретки: " << SequenceReadStream.GetPosition() << ")\n";
                        }
                        SequenceReadStream.Close();
                        break;
                    }
                    case 5: { 
                        std::cout << "Создаем пустой MutableArraySequence...\n";
                        MutableArraySequence<int> DynamicArray;
                        
                        std::cout << "Оборачиваем его в WriteOnlyStream...\n";
                        WriteOnlyStream<int> SequenceWriteStream(&DynamicArray);
                        SequenceWriteStream.Open();

                        std::cout << "Пишем значения 77, 88, 99 через поток...\n";
                        SequenceWriteStream.Write(77);
                        SequenceWriteStream.Write(88);
                        SequenceWriteStream.Write(99);
                        SequenceWriteStream.Close();

                        std::cout << "Проверяем содержимое исходного MutableArraySequence: ";
                        for (int ArrayElementIndex = 0; ArrayElementIndex < DynamicArray.GetLength(); ArrayElementIndex++) {
                            std::cout << DynamicArray.Get(ArrayElementIndex) << " ";
                        }
                        std::cout << "\n";
                        break;
                    }
                    case 0: break;
                    default: std::cout << "Неизвестная команда.\n";
                }
            } catch (const std::exception& ErrorMessage) {
                std::cout << "ОШИБКА: " << ErrorMessage.what() << "\n";
            }
        }
    }

    // --- ПОДМЕНЮ 3: Алфавитный указатель ---
    void HandleAlphabeticalIndex() {
        int IndexChoice = -1;
        while (IndexChoice != 0) {
            PrintSeparator();
            std::cout << "[ АЛФАВИТНЫЙ УКАЗАТЕЛЬ ]\n";
            std::cout << "1. Построить индекс из консольного ввода (StringStream)\n";
            std::cout << "2. Построить индекс из текстового файла (FileStream)\n";
            std::cout << "3. Вывести текущий алфавитный указатель\n";
            std::cout << "0. Назад\n";
            std::cout << "Ваш выбор: ";
            std::cin >> IndexChoice;

            if (std::cin.fail()) { ClearInputBuffer(); continue; }

            try {
                switch (IndexChoice) {
                    case 1: {
                        ClearInputBuffer();
                        std::string InputText;
                        std::cout << "Введите текст для индексации: ";
                        std::getline(std::cin, InputText);
                        
                        auto StringDeserializer = [](const std::string& word) -> std::string { return word; };
                        ReadOnlyStream<std::string> StringStreamSource(InputText, StringDeserializer);
                        
                        delete ActiveIndex;
                        ActiveIndex = new AlphabeticalIndex();
                        ActiveIndex->BuildFromStream(&StringStreamSource);
                        std::cout << "Индекс успешно построен!\n";
                        break;
                    }
                    case 2: {
                        ClearInputBuffer();
                        std::string FileName;
                        std::cout << "Введите имя файла (например, document.txt): ";
                        std::getline(std::cin, FileName);
                        
                        std::ofstream TestFile(FileName, std::ios::app);
                        TestFile.close();

                        auto StringDeserializer = [](const std::string& word) -> std::string { return word; };
                        ReadOnlyStream<std::string> FileStreamSource(FileName.c_str(), StringDeserializer);
                        FileStreamSource.Open();

                        delete ActiveIndex;
                        ActiveIndex = new AlphabeticalIndex();
                        ActiveIndex->BuildFromStream(&FileStreamSource);
                        
                        FileStreamSource.Close();
                        std::cout << "Индекс успешно построен из файла!\n";
                        break;
                    }
                    case 3: {
                        if (!ActiveIndex) { std::cout << "Индекс еще не построен!\n"; break; }
                        int UniqueWordsCount = ActiveIndex->GetUniqueWordsCount();
                        std::cout << "Уникальных слов: " << UniqueWordsCount << "\n";
                        for (int WordIndex = 0; WordIndex < UniqueWordsCount; WordIndex++) {
                            const IndexEntry* CurrentEntry = ActiveIndex->GetEntryAt(WordIndex);
                            std::cout << CurrentEntry->word << " -> позиции в потоке: [ ";
                            for (int PositionIndex = 0; PositionIndex < CurrentEntry->positions->GetLength(); PositionIndex++) {
                                std::cout << CurrentEntry->positions->Get(PositionIndex) << " ";
                            }
                            std::cout << "]\n";
                        }
                        break;
                    }
                    case 0: break;
                    default: std::cout << "Неизвестная команда.\n";
                }
            } catch (const std::exception& ErrorMessage) {
                std::cout << "ОШИБКА: " << ErrorMessage.what() << "\n";
            }
        }
    }

public:
    ConsoleInterface() : ActiveIntegerSequence(nullptr), ActiveIndex(nullptr) {}

    ~ConsoleInterface() {
        delete ActiveIntegerSequence;
        delete ActiveIndex;
    }

    void Run() {
        int MainChoice = -1;
        while (MainChoice != 0) {
            PrintSeparator();
            std::cout << "ГЛАВНОЕ МЕНЮ ЛАБОРАТОРНОЙ РАБОТЫ №4\n";
            std::cout << "1. Тестирование ленивых последовательностей и алгебры\n";
            std::cout << "2. Тестирование потоков (ReadOnly / WriteOnly Streams)\n";
            std::cout << "3. Тестирование Алфавитного Указателя\n";
            std::cout << "0. Выход\n";
            std::cout << "Ваш выбор: ";
            std::cin >> MainChoice;

            if (std::cin.fail()) {
                ClearInputBuffer();
                std::cout << "Ошибка ввода. Введите число.\n";
                continue;
            }

            switch (MainChoice) {
                case 1: HandleLazySequences(); break;
                case 2: HandleStreams(); break;
                case 3: HandleAlphabeticalIndex(); break;
                case 0: std::cout << "Завершение работы.\n"; break;
                default: std::cout << "Неверный пункт меню.\n";
            }
        }
    }
};

#endif // MENU_H