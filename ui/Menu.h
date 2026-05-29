#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include "LazySequence/LazySequence.h"
#include "Generators/GeneratorRules.h"
#include "Streams/ReadOnlyStream.h"
#include "Streams/WriteOnlyStream.h"
#include "Tasks/AlphabeticalIndex.h"
#include "sequences/MutableArraySequence.h"

class ConsoleInterface {
private:
    MutableArraySequence<LazySequence<int>*>* SequenceCollection;
    int ActiveSequenceIndex;
    AlphabeticalIndex* ActiveIndex;

    void ClearInputBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void PrintCollectionStatus() {
        if (SequenceCollection->GetLength() == 0) {
            std::cout << "Список последовательностей пуст\n";
            return;
        }
        std::cout << "Доступные последовательности:\n";
        for (int SequenceIndex = 0; SequenceIndex < SequenceCollection->GetLength(); SequenceIndex++) {
            std::cout << "[" << SequenceIndex << "] ";
            if (SequenceIndex == ActiveSequenceIndex) {
                std::cout << "(Активная) ";
            }
            if (SequenceCollection->Get(SequenceIndex)->IsInfinite()) {
                std::cout << "Бесконечная последовательность\n";
            } else {
                std::cout << "Конечная последовательность (Длина: " << SequenceCollection->Get(SequenceIndex)->GetLength() << ")\n";
            }
        }
    }

    void HandleLazySequences() {
        int SequenceChoice = -1;
        while (SequenceChoice != 0) {
            std::cout << "\nМеню тестирования ленивых последовательностей\n\n";
            std::cout << "Управление списком:\n";
            std::cout << "1. Вывести список всех созданных последовательностей\n";
            std::cout << "2. Выбрать активную последовательность по номеру\n\n";
            std::cout << "Создание базовых последовательностей:\n";
            std::cout << "3. Создать пустую конечную последовательность\n";
            std::cout << "4. Создать пустую бесконечную последовательность\n";
            std::cout << "5. Создать Арифметическую прогрессию\n";
            std::cout << "6. Создать Фибоначчи\n";
            std::cout << "7. Создать Простые числа\n\n";
            std::cout << "Операции над активной последовательностью:\n";
            std::cout << "8. Добавить элемент в конец (Append)\n";
            std::cout << "9. Вставить элемент по индексу (InsertAt)\n";
            std::cout << "10. Удалить элемент по индексу (RemoveAt)\n";
            std::cout << "11. Проверить получение крайних элементов (GetFirst / GetLast)\n";
            std::cout << "12. Проверить Map (Умножить элементы на 10 и сохранить)\n";
            std::cout << "13. Проверить Where (Оставить только четные и сохранить)\n"; 
            std::cout << "14. Проверить Reduce (Сумма первых N элементов)\n";
            std::cout << "15. Проверить Zip (Сцепить с другой последовательностью из списка)\n";
            std::cout << "16. Проверить GetSubsequence (Получить подсписок)\n";
            std::cout << "17. Вывести элементы и проверить материализацию\n";
            std::cout << "18. Проверить Concat (Сцепить с другой последовательностью из списка)\n";
            std::cout << "0. Назад в главное меню\n\n";
            std::cout << "Ваш выбор: ";
            std::cin >> SequenceChoice;

            if (std::cin.fail()) { ClearInputBuffer(); continue; }

            try {
                switch (SequenceChoice) {
                    case 1: {
                        PrintCollectionStatus();
                        break;
                    }
                    case 2: {
                        PrintCollectionStatus();
                        if (SequenceCollection->GetLength() > 0) {
                            int TargetIndex;
                            std::cout << "Введите номер последовательности для работы: ";
                            std::cin >> TargetIndex;
                            if (TargetIndex >= 0 && TargetIndex < SequenceCollection->GetLength()) {
                                ActiveSequenceIndex = TargetIndex;
                                std::cout << "Активная последовательность изменена\n";
                            } else {
                                std::cout << "Неверный номер\n";
                            }
                        }
                        break;
                    }
                    case 3: {
                        LazySequence<int>* EmptyFinite = new LazySequence<int>();
                        SequenceCollection->Append(EmptyFinite);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Пустая конечная последовательность создана и назначена активной\n";
                        break;
                    }
                    case 4: {
                        auto ThrowingRule = [](Sequence<int>* self) -> int {
                            throw std::out_of_range("Эта бесконечная последовательность не содержит элементов по правилу");
                        };
                        LazySequence<int>* EmptyInfinite = new LazySequence<int>(ThrowingRule, Ordinal::Infinite());
                        SequenceCollection->Append(EmptyInfinite);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Пустая бесконечная последовательность создана и назначена активной\n";
                        break;
                    }
                    case 5: {
                        int StartValue, StepValue, IsInfiniteFlag;
                        std::cout << "Введите начальное значение: "; std::cin >> StartValue;
                        std::cout << "Введите шаг: "; std::cin >> StepValue;
                        std::cout << "Сделать последовательность бесконечной? (1 - Да, 0 - Нет): "; std::cin >> IsInfiniteFlag;
                        
                        Ordinal SequenceCapacity;
                        if (IsInfiniteFlag == 1) {
                            SequenceCapacity = Ordinal::Infinite();
                        } else {
                            int SequenceLength;
                            std::cout << "Введите желаемую длину последовательности: "; std::cin >> SequenceLength;
                            SequenceCapacity = Ordinal(SequenceLength);
                        }

                        LazySequence<int>* ArithmeticSeq = new LazySequence<int>(CreateArithmeticProgression(StartValue, StepValue), SequenceCapacity);
                        SequenceCollection->Append(ArithmeticSeq);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Арифметическая прогрессия создана и назначена активной\n";
                        break;
                    }
                    case 6: {
                        int IsInfiniteFlag;
                        std::cout << "Сделать последовательность Фибоначчи бесконечной? (1 - Да, 0 - Нет): "; std::cin >> IsInfiniteFlag;
                        
                        Ordinal SequenceCapacity;
                        if (IsInfiniteFlag == 1) {
                            SequenceCapacity = Ordinal::Infinite();
                        } else {
                            int SequenceLength;
                            std::cout << "Введите желаемую длину последовательности: "; std::cin >> SequenceLength;
                            SequenceCapacity = Ordinal(SequenceLength);
                        }

                        LazySequence<int>* FibonacciSeq = new LazySequence<int>(CreateFibonacciSequence(), SequenceCapacity);
                        SequenceCollection->Append(FibonacciSeq);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Последовательность Фибоначчи создана и назначена активной\n";
                        break;
                    }
                    case 7: {
                        int IsInfiniteFlag;
                        std::cout << "Сделать последовательность простых чисел бесконечной? (1 - Да, 0 - Нет): "; std::cin >> IsInfiniteFlag;
                        
                        Ordinal SequenceCapacity;
                        if (IsInfiniteFlag == 1) {
                            SequenceCapacity = Ordinal::Infinite();
                        } else {
                            int SequenceLength;
                            std::cout << "Введите желаемую длину последовательности: "; std::cin >> SequenceLength;
                            SequenceCapacity = Ordinal(SequenceLength);
                        }

                        LazySequence<int>* PrimeSeq = new LazySequence<int>(CreatePrimeSequence(), SequenceCapacity);
                        SequenceCollection->Append(PrimeSeq);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Последовательность простых чисел создана и назначена активной\n";
                        break;
                    }
                    case 8: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        int ElementToAdd;
                        std::cout << "Введите значение для добавления: ";
                        std::cin >> ElementToAdd;
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        Sequence<int>* MutatedSequence = CurrentSequence->Append(ElementToAdd);
                        
                        SequenceCollection->Append(static_cast<LazySequence<int>*>(MutatedSequence));
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Элемент успешно добавлен в конец. Результат сохранен как новая активная последовательность\n";
                        break;
                    }
                    case 9: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        int ElementToAdd, TargetPosition;
                        std::cout << "Введите значение: "; std::cin >> ElementToAdd;
                        std::cout << "Введите индекс для вставки: "; std::cin >> TargetPosition;
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        Sequence<int>* MutatedSequence = CurrentSequence->InsertAt(ElementToAdd, TargetPosition);
                        
                        SequenceCollection->Append(static_cast<LazySequence<int>*>(MutatedSequence));
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Элемент вставлен. Результат сохранен как новая активная последовательность\n";
                        break;
                    }
                    case 10: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        int TargetPosition;
                        std::cout << "Введите индекс для удаления: "; std::cin >> TargetPosition;
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        LazySequence<int>* MutatedSequence = CurrentSequence->RemoveAt(TargetPosition);
                        
                        SequenceCollection->Append(MutatedSequence);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Элемент удален. Результат сохранен как новая активная последовательность\n";
                        break;
                    }
                    case 11: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        std::cout << "Первый элемент: " << CurrentSequence->GetFirst() << "\n";
                        if (CurrentSequence->IsInfinite()) {
                            std::cout << "Последовательность бесконечна, метод GetLast вызовет ошибку логики\n";
                        } else {
                            std::cout << "Последний элемент: " << CurrentSequence->GetLast() << "\n";
                        }
                        break;
                    }
                    case 12: { 
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        auto MultiplyByTen = [](const int& CurrentValue) -> int { return CurrentValue * 10; };
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        LazySequence<int>* MappedSequence = CurrentSequence->Map<int>(MultiplyByTen);
                        
                        SequenceCollection->Append(MappedSequence);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        
                        std::cout << "Map (умножение на 10) успешно применен.\n";
                        std::cout << "Результат сохранен как новая активная последовательность!\n";
                        break;
                    }
                    case 13: { 
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        auto IsEven = [](const int& CurrentValue) -> bool { return CurrentValue % 2 == 0; };
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        LazySequence<int>* FilteredSequence = CurrentSequence->Where(IsEven);
                        
                        SequenceCollection->Append(FilteredSequence);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        
                        std::cout << "Where (только четные) успешно применен.\n";
                        std::cout << "Результат (бесконечный генератор четных) сохранен как новая активная последовательность!\n";
                        break;
                    }
                    case 14: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        int ElementCount;
                        std::cout << "Сколько первых элементов просуммировать: ";
                        std::cin >> ElementCount;
                        
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        LazySequence<int>* SubSequence = static_cast<LazySequence<int>*>(CurrentSequence->GetSubsequence(0, ElementCount - 1));
                        auto SumReducer = [](const int& AccumulatedValue, const int& CurrentValue) -> int {
                            return AccumulatedValue + CurrentValue;
                        };
                        
                        int TotalSum = SubSequence->Reduce(SumReducer, 0);
                        std::cout << "Результат свертки Reduce: " << TotalSum << "\n";
                        delete SubSequence;
                        break;
                    }
                    case 15: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        PrintCollectionStatus();
                        int SecondTargetIndex;
                        std::cout << "Выберите номер второй последовательности для Zip: ";
                        std::cin >> SecondTargetIndex;
                        
                        if (SecondTargetIndex >= 0 && SecondTargetIndex < SequenceCollection->GetLength()) {
                            LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                            LazySequence<int>* SecondSequence = SequenceCollection->Get(SecondTargetIndex);
                            
                            int PrintCount;
                            std::cout << "Сколько пар вывести на экран? ";
                            std::cin >> PrintCount;

                            auto ZippedSequence = CurrentSequence->Zip(SecondSequence);
                            std::cout << "Результат применения Zip:\n";
                            try {
                                for (int PrintIndex = 0; PrintIndex < PrintCount; PrintIndex++) {
                                    auto CurrentPair = ZippedSequence->Get(PrintIndex);
                                    std::cout << "[" << PrintIndex << "]: " << CurrentPair.first << " | " << CurrentPair.second << "\n";
                                }
                            } catch (const std::out_of_range&) {
                                std::cout << "(Достигнут конец одной из последовательностей)\n";
                            }
                            delete ZippedSequence;
                        } else {
                            std::cout << "Неверный номер\n";
                        }
                        break;
                    }
                    case 16: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        int StartIndex, EndIndex;
                        std::cout << "Начальный индекс: "; std::cin >> StartIndex;
                        std::cout << "Конечный индекс: "; std::cin >> EndIndex;
                        
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        LazySequence<int>* SubSequence = static_cast<LazySequence<int>*>(CurrentSequence->GetSubsequence(StartIndex, EndIndex));
                        
                        SequenceCollection->Append(SubSequence);
                        ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                        std::cout << "Подсписок извлечен и сохранен как новая активная последовательность\n";
                        break;
                    }
                    case 17: {
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        int PrintCount;
                        std::cout << "Сколько элементов вывести на экран: "; std::cin >> PrintCount;
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        for (int PrintIndex = 0; PrintIndex < PrintCount; PrintIndex++) {
                            std::cout << CurrentSequence->Get(PrintIndex) << " ";
                        }
                        std::cout << "\nКоличество материализованных элементов в кэше: " << CurrentSequence->GetMaterializedCount() << "\n";
                        break;
                    }
                    case 18: { 
                        if (ActiveSequenceIndex == -1) { std::cout << "Нет активной последовательности\n"; break; }
                        PrintCollectionStatus();
                        int SecondTargetIndex;
                        std::cout << "Выберите номер второй последовательности для Concat: ";
                        std::cin >> SecondTargetIndex;
                        
                        if (SecondTargetIndex >= 0 && SecondTargetIndex < SequenceCollection->GetLength()) {
                            LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                            LazySequence<int>* SecondSequence = SequenceCollection->Get(SecondTargetIndex);
                            
                            LazySequence<int>* ConcatenatedSequence = static_cast<LazySequence<int>*>(CurrentSequence->Concat(SecondSequence));
                            SequenceCollection->Append(ConcatenatedSequence);
                            ActiveSequenceIndex = SequenceCollection->GetLength() - 1;
                            
                            std::cout << "Последовательности объединены. Результат сохранен как новая активная последовательность\n";
                            
                            int ChunkDelta, LocalIndex;
                            std::cout << "Введите дельту для чанка проверки (omega): ";
                            std::cin >> ChunkDelta;
                            std::cout << "Введите локальный индекс в выбранном чанке: ";
                            std::cin >> LocalIndex;

                            try {
                                int ResultValue = ConcatenatedSequence->Get(Ordinal(ChunkDelta, LocalIndex));
                                std::cout << "Значение по ординалу " << ChunkDelta << " и индексу " << LocalIndex << " составляет: " << ResultValue << "\n";
                            } catch (const std::exception& ErrorMessage) {
                                std::cout << "Ошибка доступа по ординалу: " << ErrorMessage.what() << "\n";
                            }
                        } else {
                            std::cout << "Неверный номер\n";
                        }
                        break;
                    }
                    case 0: break;
                    default: std::cout << "Команда не распознана\n";
                }
            } catch (const std::exception& ErrorMessage) {
                std::cout << "Произошла ошибка: " << ErrorMessage.what() << "\n";
            }
        }
    }

    void HandleStreams() {
        int StreamChoice = -1;
        while (StreamChoice != 0) {
            std::cout << "\nМеню тестирования системы потоков\n\n";
            std::cout << "1. Записать элементы в файл (FileTarget)\n";
            std::cout << "2. Прочитать элементы из файла (FileSource)\n";
            std::cout << "3. Прочитать строку (StringSource)\n";
            std::cout << "4. Проверить перемещение каретки (Seek) в строке\n";
            std::cout << "5. Прочитать из ленивой последовательности (ReadOnlyStream)\n";
            std::cout << "6. Записать в динамический массив (WriteOnlyStream)\n";
            std::cout << "0. Назад в главное меню\n\n";
            std::cout << "Ваш выбор: ";
            std::cin >> StreamChoice;

            if (std::cin.fail()) { ClearInputBuffer(); continue; }

            try {
                switch (StreamChoice) {
                    case 1: {
                        std::string OutputFileName = "test_stream_output.txt";
                        auto IntegerSerializer = [](const int& CurrentValue) -> std::string { return std::to_string(CurrentValue); };
                        
                        WriteOnlyStream<int> FileTargetStream(OutputFileName.c_str(), IntegerSerializer);
                        FileTargetStream.Open();
                        
                        std::cout << "Запись значений 100, 200, 300 в файл\n";
                        FileTargetStream.Write(100);
                        FileTargetStream.Write(200);
                        FileTargetStream.Write(300);
                        
                        FileTargetStream.Close(); 
                        std::cout << "Данные успешно сохранены в файл, поток закрыт\n";
                        break;
                    }
                    case 2: {
                        std::string InputFileName = "test_stream_output.txt";
                        auto IntegerDeserializer = [](const std::string& ParsedText) -> int { return std::stoi(ParsedText); };
                        
                        ReadOnlyStream<int> FileSourceStream(InputFileName.c_str(), IntegerDeserializer);
                        FileSourceStream.Open();
                        
                        std::cout << "Чтение данных из файла:\n";
                        while (!FileSourceStream.IsEndOfStream()) {
                            try {
                                int ReadValue = FileSourceStream.Read();
                                std::cout << "Получено значение: " << ReadValue << " на позиции " << FileSourceStream.GetPosition() << "\n";
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
                        std::cout << "Введите строку чисел разделенных пробелом: ";
                        std::getline(std::cin, InputText);
                        
                        auto IntegerDeserializer = [](const std::string& ParsedText) -> int { return std::stoi(ParsedText); };
                        ReadOnlyStream<int> StringSourceStream(InputText, IntegerDeserializer);
                        StringSourceStream.Open();
                        
                        std::cout << "Чтение значений из строкового потока:\n";
                        while (!StringSourceStream.IsEndOfStream()) {
                            try {
                                int ReadValue = StringSourceStream.Read();
                                std::cout << "Считано значение: " << ReadValue << "\n";
                            } catch (const std::out_of_range&) {
                                break;
                            }
                        }
                        StringSourceStream.Close();
                        break;
                    }
                    case 4: {
                        std::string TestText = "11 22 33 44 55";
                        auto IntegerDeserializer = [](const std::string& ParsedText) -> int { return std::stoi(ParsedText); };
                        ReadOnlyStream<int> StringSeekStream(TestText, IntegerDeserializer);
                        StringSeekStream.Open();
                        
                        std::cout << "Исходная строка для теста каретки: " << TestText << "\n";
                        std::cout << "Перемещаем каретку на индекс 3\n";
                        StringSeekStream.Seek(3);
                        std::cout << "Прочитанное значение должно быть 44. Фактическое значение: " << StringSeekStream.Read() << "\n";
                        StringSeekStream.Close();
                        break;
                    }
                    case 5: { 
                        if (ActiveSequenceIndex == -1) { std::cout << "Необходимо создать последовательность в первом меню\n"; break; }
                        
                        LazySequence<int>* CurrentSequence = SequenceCollection->Get(ActiveSequenceIndex);
                        ReadOnlyStream<int> SequenceReadStream(CurrentSequence);
                        SequenceReadStream.Open(); 
                        
                        int ElementsToRead;
                        std::cout << "Количество элементов для чтения из потока: ";
                        std::cin >> ElementsToRead;

                        for (int ReadIndex = 0; ReadIndex < ElementsToRead; ReadIndex++) {
                            if (SequenceReadStream.IsEndOfStream()) {
                                std::cout << "Достигнут конец последовательности\n";
                                break;
                            }
                            std::cout << "Прочитано потоком: " << SequenceReadStream.Read() << " текущая позиция " << SequenceReadStream.GetPosition() << "\n";
                        }
                        
                        std::cout << "Принудительное закрытие потока... ";
                        SequenceReadStream.Close();
                        if (SequenceReadStream.IsEndOfStream()) {
                            std::cout << "Успех! Поток сигнализирует о завершении (EOF).\n";
                        }
                        break;
                    }
                    case 6: { 
                        MutableArraySequence<int> DynamicTargetArray;
                        WriteOnlyStream<int> SequenceWriteStream(&DynamicTargetArray);
                        SequenceWriteStream.Open();

                        std::cout << "Передача значений 77, 88, 99 через абстракцию потока\n";
                        SequenceWriteStream.Write(77);
                        SequenceWriteStream.Write(88);
                        SequenceWriteStream.Write(99);
                        SequenceWriteStream.Close(); 

                        std::cout << "Попытка записать значение 100 в закрытый поток...\n";
                        try {
                            SequenceWriteStream.Write(100);
                        } catch (const std::exception& e) {
                            std::cout << "Перехвачено исключение (как и ожидалось): " << e.what() << "\n";
                        }

                        std::cout << "Проверка состояния базового массива (должно быть только 77 88 99): ";
                        for (int ArrayElementIndex = 0; ArrayElementIndex < DynamicTargetArray.GetLength(); ArrayElementIndex++) {
                            std::cout << DynamicTargetArray.Get(ArrayElementIndex) << " ";
                        }
                        std::cout << "\n";
                        break;
                    }
                    case 0: break;
                    default: std::cout << "Команда не распознана\n";
                }
            } catch (const std::exception& ErrorMessage) {
                std::cout << "Произошла ошибка: " << ErrorMessage.what() << "\n";
            }
        }
    }

    void HandleAlphabeticalIndex() {
        int IndexChoice = -1;
        while (IndexChoice != 0) {
            std::cout << "\nМеню тестирования алфавитного указателя\n\n";
            std::cout << "1. Построить указатель из консольного ввода\n";
            std::cout << "2. Построить указатель из текстового файла\n";
            std::cout << "3. Построить указатель из ленивой последовательности\n";
            std::cout << "4. Вывести текущий алфавитный указатель на экран\n";
            std::cout << "0. Назад в главное меню\n\n";
            std::cout << "Ваш выбор: ";
            std::cin >> IndexChoice;

            if (std::cin.fail()) { ClearInputBuffer(); continue; }

            try {
                switch (IndexChoice) {
                    case 1: {
                        ClearInputBuffer();
                        std::string InputText;
                        std::cout << "Введите текст для построения индекса: ";
                        std::getline(std::cin, InputText);
                        
                        auto StringDeserializer = [](const std::string& ParsedWord) -> std::string { return ParsedWord; };
                        ReadOnlyStream<std::string> StringStreamSource(InputText, StringDeserializer);
                        StringStreamSource.Open();
                        
                        delete ActiveIndex;
                        ActiveIndex = new AlphabeticalIndex();
                        ActiveIndex->BuildFromStream(&StringStreamSource);
                        StringStreamSource.Close();
                        std::cout << "Алфавитный указатель успешно построен\n";
                        break;
                    }
                    case 2: {
                        ClearInputBuffer();
                        std::string TargetFileName;
                        std::cout << "Укажите имя файла: ";
                        std::getline(std::cin, TargetFileName);
                        
                        std::ofstream TestFile(TargetFileName, std::ios::app);
                        TestFile.close();

                        auto StringDeserializer = [](const std::string& ParsedWord) -> std::string { return ParsedWord; };
                        ReadOnlyStream<std::string> FileStreamSource(TargetFileName.c_str(), StringDeserializer);
                        FileStreamSource.Open();

                        delete ActiveIndex;
                        ActiveIndex = new AlphabeticalIndex();
                        ActiveIndex->BuildFromStream(&FileStreamSource);
                        
                        FileStreamSource.Close();
                        std::cout << "Алфавитный указатель построен на основе файла\n";
                        break;
                    }
                    case 3: {
                        std::string ArrayValues[] = {"apple", "banana", "apple", "cherry"};
                        LazySequence<std::string> StringLazySequence(ArrayValues, 4);

                        delete ActiveIndex;
                        ActiveIndex = new AlphabeticalIndex();
                        ActiveIndex->BuildFromLazySequence(&StringLazySequence);
                        
                        std::cout << "Алфавитный указатель построен на основе ленивой последовательности\n";
                        break;
                    }
                    case 4: {
                        if (!ActiveIndex) { std::cout << "Указатель не содержит данных\n"; break; }
                        int UniqueWordsCount = ActiveIndex->GetUniqueWordsCount();
                        std::cout << "Найдено уникальных слов: " << UniqueWordsCount << "\n";
                        for (int WordIndex = 0; WordIndex < UniqueWordsCount; WordIndex++) {
                            const IndexEntry* CurrentEntry = ActiveIndex->GetEntryAt(WordIndex);
                            std::cout << CurrentEntry->word << " зафиксировано на позициях: ";
                            for (int PositionIndex = 0; PositionIndex < CurrentEntry->positions->GetLength(); PositionIndex++) {
                                std::cout << CurrentEntry->positions->Get(PositionIndex) << " ";
                            }
                            std::cout << "\n";
                        }
                        break;
                    }
                    case 0: break;
                    default: std::cout << "Команда не распознана\n";
                }
            } catch (const std::exception& ErrorMessage) {
                std::cout << "Произошла ошибка: " << ErrorMessage.what() << "\n";
            }
        }
    }

public:
    ConsoleInterface() : ActiveSequenceIndex(-1), ActiveIndex(nullptr) {
        SequenceCollection = new MutableArraySequence<LazySequence<int>*>();
    }

    ~ConsoleInterface() {
        for (int SequenceIndex = 0; SequenceIndex < SequenceCollection->GetLength(); SequenceIndex++) {
            delete SequenceCollection->Get(SequenceIndex);
        }
        delete SequenceCollection;
        delete ActiveIndex;
    }

    void Run() {
        int MainChoice = -1;
        while (MainChoice != 0) {
            std::cout << "\nГлавное меню лабораторной работы\n";
            std::cout << "1. Тестирование ленивых последовательностей и алгебры\n";
            std::cout << "2. Тестирование системы потоков\n";
            std::cout << "3. Тестирование алгоритма алфавитного указателя\n";
            std::cout << "0. Завершить выполнение\n";
            std::cout << "Ваш выбор: ";
            std::cin >> MainChoice;

            if (std::cin.fail()) {
                ClearInputBuffer();
                std::cout << "Введено некорректное значение. Ожидается число\n";
                continue;
            }

            switch (MainChoice) {
                case 1: HandleLazySequences(); break;
                case 2: HandleStreams(); break;
                case 3: HandleAlphabeticalIndex(); break;
                case 0: std::cout << "Работа программы завершена\n"; break;
                default: std::cout << "Введен неверный пункт меню\n";
            }
        }
    }
};

#endif // MENU_H
