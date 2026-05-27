#include "tests/Tests.h"
#include <iostream>
#include <cassert>
#include <string>
#include <cstdio>
#include <functional>

#include "LazySequence/LazySequence.h"
#include "Generators/GeneratorRules.h"
#include "Streams/ReadOnlyStream.h"
#include "Streams/WriteOnlyStream.h"
#include "Tasks/AlphabeticalIndex.h"
#include "sequences/MutableArraySequence.h"
#include "Ordinal/Ordinal.h"

#define ASSERT_TEST(ConditionToCheck, TestMessage) \
    if (!(ConditionToCheck)) { \
        std::cerr << "что-то пошло не так.....: " << TestMessage << std::endl; \
        assert(ConditionToCheck); \
    } else { \
        std::cout << "успех, отлично: " << TestMessage << std::endl; \
    }
// тест генератора
void TestGeneratorRules() {
    std::cout << "Начало тестирования правил генераторов" << std::endl;

    auto ArithmeticRule = CreateArithmeticProgression(5, 5);
    LazySequence<int> ArithmeticSequence(ArithmeticRule, Ordinal::Infinite());
    ASSERT_TEST(ArithmeticSequence.Get(0) == 5, "Арифметическая прогрессия: первый элемент равен 5");
    ASSERT_TEST(ArithmeticSequence.Get(1) == 10, "Арифметическая прогрессия: второй элемент равен 10");
    ASSERT_TEST(ArithmeticSequence.Get(10) == 55, "Арифметическая прогрессия: одиннадцатый элемент равен 55");

    auto GeometricRule = CreateGeometricProgression(2, 3);
    LazySequence<int> GeometricSequence(GeometricRule, Ordinal::Infinite());
    ASSERT_TEST(GeometricSequence.Get(0) == 2, "Геометрическая прогрессия: первый элемент равен 2");
    ASSERT_TEST(GeometricSequence.Get(1) == 6, "Геометрическая прогрессия: второй элемент равен 6");
    ASSERT_TEST(GeometricSequence.Get(3) == 54, "Геометрическая прогрессия: четвертый элемент равен 54");

    auto FibonacciRule = CreateFibonacciSequence();
    LazySequence<int> FibonacciSequence(FibonacciRule, Ordinal::Infinite());
    ASSERT_TEST(FibonacciSequence.Get(0) == 0, "Фибоначчи бесконечная: первый элемент равен 0");
    ASSERT_TEST(FibonacciSequence.Get(7) == 13, "Фибоначчи бесконечная: восьмой элемент равен 13");

    auto PrimeRule = CreatePrimeSequence();
    LazySequence<int> PrimeSequence(PrimeRule, Ordinal::Infinite());
    ASSERT_TEST(PrimeSequence.Get(0) == 2, "Простые числа: первый элемент равен 2");
    ASSERT_TEST(PrimeSequence.Get(7) == 19, "Простые числа: восьмой элемент равен 19");

    
    auto FiniteFibonacciRule = CreateFibonacciSequence();
    LazySequence<int> FiniteFibonacciSequence(FiniteFibonacciRule, Ordinal(5));
    ASSERT_TEST(FiniteFibonacciSequence.GetLength() == 5, "Фибоначчи конечная: длина равна 5");
    ASSERT_TEST(FiniteFibonacciSequence.GetLast() == 3, "Фибоначчи конечная: последний элемент равен 3");

    auto SumAccumulator = [](const int& AccumulatedSum, const int& CurrentValue) { return AccumulatedSum + CurrentValue; };
    int TotalFiniteSum = FiniteFibonacciSequence.Reduce(SumAccumulator, 0); 
    ASSERT_TEST(TotalFiniteSum == 7, "Reduce конечной генерации: сумма элементов (0+1+1+2+3) равна 7");
    
    std::cout << "Завершение тестирования правил генераторов" << std::endl;
}

void TestLazySequence() {
    std::cout << "Начало тестирования ленивых последовательностей и алгебры" << std::endl;

    auto BaseRule = CreateArithmeticProgression(1, 1); 
    LazySequence<int> BaseSequence(BaseRule, Ordinal::Infinite());

    ASSERT_TEST(BaseSequence.IsInfinite() == true, "Проверка флага бесконечности");
    ASSERT_TEST(BaseSequence.GetFirst() == 1, "Получение первого элемента");

    LazySequence<int>* SubSequence = static_cast<LazySequence<int>*>(BaseSequence.GetSubsequence(0, 4));
    ASSERT_TEST(SubSequence->GetLength() == 5, "Длина извлеченного подсписка равна 5");
    ASSERT_TEST(SubSequence->Get(0) == 1, "Первый элемент подписка равен 1");
    ASSERT_TEST(SubSequence->GetLast() == 5, "Последний элемент подсписка равен 5");

    auto MultiplyByTwo = [](const int& CurrentValue) { return CurrentValue * 2; };
    LazySequence<int>* MappedSequence = SubSequence->Map<int>(MultiplyByTwo); 
    ASSERT_TEST(MappedSequence->Get(0) == 2, "Map: первый элемент умножен на 2");
    ASSERT_TEST(MappedSequence->Get(4) == 10, "Map: пятый элемент умножен на 2");

    auto CheckIsEven = [](const int& CurrentValue) { return CurrentValue % 2 == 0; };
    LazySequence<int>* FilteredSequence = SubSequence->Where(CheckIsEven); 
    ASSERT_TEST(FilteredSequence->Get(0) == 2, "Where: первый четный элемент равен 2");
    ASSERT_TEST(FilteredSequence->Get(1) == 4, "Where: второй четный элемент равен 4");

    auto SumAccumulator = [](const int& AccumulatedSum, const int& CurrentValue) { return AccumulatedSum + CurrentValue; };
    int TotalSum = SubSequence->Reduce(SumAccumulator, 0); 
    ASSERT_TEST(TotalSum == 15, "Reduce: сумма первых пяти элементов равна 15");

    Sequence<int>* AppendedSequence = SubSequence->Append(100);
    ASSERT_TEST(AppendedSequence->GetLength() == 6, "Append: длина новой последовательности стала 6");
    ASSERT_TEST(AppendedSequence->Get(5) == 100, "Append: последний элемент равен 100");
    ASSERT_TEST(SubSequence->GetLength() == 5, "Иммутабельность: длина оригинального подсписка осталась 5");

    LazySequence<int> AdditionalSequence(CreateArithmeticProgression(10, 10), Ordinal::Infinite());
    
    auto ZippedSequence = SubSequence->Zip(&AdditionalSequence);
    ASSERT_TEST(ZippedSequence->Get(0).first == 1 && ZippedSequence->Get(0).second == 10, "Zip: корректное сцепление первой пары");

    int FirstArrayValues[] = {10, 20, 30};
    LazySequence<int> FirstChunkSequence(FirstArrayValues, 3);
    int SecondArrayValues[] = {40, 50};
    LazySequence<int> SecondChunkSequence(SecondArrayValues, 2);
    
    auto ConcatenatedSequence = static_cast<LazySequence<int>*>(FirstChunkSequence.Concat(&SecondChunkSequence));
    
    ASSERT_TEST(ConcatenatedSequence->GetLength() == 5, "Concat: общая длина конкатенированной последовательности равна 5");
    
    ASSERT_TEST(ConcatenatedSequence->Get(Ordinal(0, 2)) == 30, "Concat: плоский индекс 2 берется из первого чанка = 30");
    ASSERT_TEST(ConcatenatedSequence->Get(Ordinal(0, 3)) == 40, "Concat (Ординалы): индекс 3 перетекает во второй чанк = 40");
    ASSERT_TEST(ConcatenatedSequence->Get(Ordinal(0, 4)) == 50, "Concat (Ординалы): индекс 4 перетекает во второй чанк = 50");

    delete SubSequence;
    delete MappedSequence;
    delete FilteredSequence;
    delete AppendedSequence;
    delete ZippedSequence;
    delete ConcatenatedSequence;
    
    std::cout << "Завершение тестирования ленивых последовательностей" << std::endl;
}
// тест стримов
void TestStreams() {
    std::cout << "Начало тестирования системы потоков" << std::endl;

    auto StringDeserializer = [](const std::string& ParsedString) { return ParsedString; };
    ReadOnlyStream<std::string> StringReadStream(std::string("Word1 Word2 Word3"), StringDeserializer);
    StringReadStream.Open();
    ASSERT_TEST(StringReadStream.Read() == "Word1", "Чтение первого слова из строкового потока");
    ASSERT_TEST(StringReadStream.Read() == "Word2", "Чтение второго слова из строкового потока");
    ASSERT_TEST(StringReadStream.GetPosition() == 2, "Проверка позиции каретки строкового потока");
    StringReadStream.Seek(0);
    ASSERT_TEST(StringReadStream.Read() == "Word1", "Чтение после сброса каретки на начало");
    StringReadStream.Close();

    MutableArraySequence<int> DynamicTargetArray;
    WriteOnlyStream<int> ArrayWriteStream(&DynamicTargetArray);
    ArrayWriteStream.Open();
    ArrayWriteStream.Write(10);
    ArrayWriteStream.Write(20);
    ArrayWriteStream.Write(30);
    ASSERT_TEST(ArrayWriteStream.GetPosition() == 3, "Проверка позиции каретки потока записи");
    ArrayWriteStream.Close();
    ASSERT_TEST(DynamicTargetArray.GetLength() == 3, "Проверка длины целевого массива после записи");
    ASSERT_TEST(DynamicTargetArray.Get(1) == 20, "Проверка сохраненного элемента в целевом массиве");

    std::string TestFileName = "test_streams_output.txt";
    auto IntegerSerializer = [](const int& CurrentValue) { return std::to_string(CurrentValue); };
    auto IntegerDeserializer = [](const std::string& ParsedString) { return std::stoi(ParsedString); };

    WriteOnlyStream<int> FileWriteStream(TestFileName.c_str(), IntegerSerializer);
    FileWriteStream.Open();
    FileWriteStream.Write(123);
    FileWriteStream.Write(456);
    FileWriteStream.Close();

    ReadOnlyStream<int> FileReadStream(TestFileName.c_str(), IntegerDeserializer);
    FileReadStream.Open();
    ASSERT_TEST(FileReadStream.Read() == 123, "Чтение первого числа из файлового потока");
    ASSERT_TEST(FileReadStream.Read() == 456, "Чтение второго числа из файлового потока");
    ASSERT_TEST(FileReadStream.IsEndOfStream() == true, "Файловый поток корректно достиг конца");
    FileReadStream.Close();

    std::remove(TestFileName.c_str());
    
    std::cout << "Завершение тестирования системы потоков" << std::endl;
}

// тест алфавитного указателя
void TestAlphabeticalIndex() {
    std::cout << "Начало тестирования алфавитного указателя" << std::endl;

    AlphabeticalIndex WordIndex;
    auto StringDeserializer = [](const std::string& ParsedString) { return ParsedString; };
    ReadOnlyStream<std::string> IndexStream(std::string("Apple banana apple cherry. Banana is good."), StringDeserializer);

    WordIndex.BuildFromStream(&IndexStream);

    ASSERT_TEST(WordIndex.GetUniqueWordsCount() == 5, "Указатель распознал 5 уникальных слов");

    const IndexEntry* FirstEntry = WordIndex.GetEntryAt(0);
    ASSERT_TEST(FirstEntry->word == "apple", "Первое слово по алфавиту apple");
    ASSERT_TEST(FirstEntry->positions->GetLength() == 2, "Слово apple зафиксировано 2 раза");

    auto ArrayRule = [ItemCounter = 0](Sequence<std::string>* self) mutable { 
        std::string SourceWords[] = {"one", "two", "three", "one"};
        if (ItemCounter >= 4) throw std::out_of_range("Конец последовательности");
        return SourceWords[ItemCounter++];
    };
    LazySequence<std::string> LazyWordSequence(ArrayRule, Ordinal(4));
    AlphabeticalIndex LazyWordIndex;
    LazyWordIndex.BuildFromLazySequence(&LazyWordSequence);
    ASSERT_TEST(LazyWordIndex.GetUniqueWordsCount() == 3, "Указатель из ленивой последовательности распознал 3 уникальных слова");
    
    std::cout << "Завершение тестирования алфавитного указателя" << std::endl;
}

void TestInsertSequenceAt() {
    std::cout << "Начало тестирования структурной вставки последовательностей" << std::endl;


    int BaseValues[] = {10, 20, 30, 40};
    LazySequence<int> BaseSeq(BaseValues, 4);

    int InsertValues[] = {99, 88};
    LazySequence<int> InsertSeq(InsertValues, 2);

    LazySequence<int>* FiniteResult = BaseSeq.InsertSequenceAt(&InsertSeq, 2);

    ASSERT_TEST(FiniteResult->GetLength() == 6, "Конечная вставка: Общая длина стала 6");
    
    ASSERT_TEST(FiniteResult->Get(Ordinal(0, 0)) == 10, "Конечная вставка: Голова, индекс 0 = 10");
    ASSERT_TEST(FiniteResult->Get(Ordinal(0, 1)) == 20, "Конечная вставка: Голова, индекс 1 = 20");
    ASSERT_TEST(FiniteResult->Get(Ordinal(0, 2)) == 99, "Конечная вставка: Перетекание во вставку, индекс 2 = 99");
    ASSERT_TEST(FiniteResult->Get(Ordinal(0, 3)) == 88, "Конечная вставка: Перетекание во вставку, индекс 3 = 88");
    ASSERT_TEST(FiniteResult->Get(Ordinal(0, 4)) == 30, "Конечная вставка: Перетекание в хвост, индекс 4 = 30");
    ASSERT_TEST(FiniteResult->Get(Ordinal(0, 5)) == 40, "Конечная вставка: Перетекание в хвост, индекс 5 = 40");

    delete FiniteResult;


   
    auto ArithmeticRule = CreateArithmeticProgression(100, 10); 
    LazySequence<int> InfiniteInsert(ArithmeticRule, Ordinal::Infinite());
    
   
    LazySequence<int>* InfiniteResult = BaseSeq.InsertSequenceAt(&InfiniteInsert, 1);

    ASSERT_TEST(InfiniteResult->IsInfinite() == true, "Вставка бесконечности: Результат бесконечен");
    
    ASSERT_TEST(InfiniteResult->Get(Ordinal(0, 0)) == 10, "Бесконечная вставка: Голова, индекс 0 = 10");
    
    ASSERT_TEST(InfiniteResult->Get(Ordinal(0, 1)) == 100, "Бесконечная вставка: Перетекание во вставку, индекс 1 = 100");
    ASSERT_TEST(InfiniteResult->Get(Ordinal(0, 2)) == 110, "Бесконечная вставка: Генерация вставки, индекс 2 = 110");
    ASSERT_TEST(InfiniteResult->Get(Ordinal(0, 5)) == 140, "Бесконечная вставка: Генерация вставки, индекс 5 = 140");
    
    ASSERT_TEST(InfiniteResult->Get(Ordinal(1, 0)) == 20, "Бесконечная вставка: Прыжок к хвосту (Омега 1), индекс 0 = 20");
    ASSERT_TEST(InfiniteResult->Get(Ordinal(1, 1)) == 30, "Бесконечная вставка: Прыжок к хвосту (Омега 1), индекс 1 = 30");
    ASSERT_TEST(InfiniteResult->Get(Ordinal(1, 2)) == 40, "Бесконечная вставка: Прыжок к хвосту (Омега 1), индекс 2 = 40");

    delete InfiniteResult;

    std::cout << "Завершение тестирования структурной вставки\n" << std::endl;
}
  

//запуск всех тестов
void RunAllTests() {
    std::cout << "\nЗАПУСК КОМПЛЕКСНОГО ТЕСТИРОВАНИЯ СИСТЕМЫ\n" << std::endl;

    try {
        TestGeneratorRules();
        std::cout << std::endl;
        TestLazySequence();
        std::cout << std::endl;
        TestInsertSequenceAt();
         std::cout << std::endl;
        TestStreams();
        std::cout << std::endl;
        TestAlphabeticalIndex();

        std::cout << "\nВСЕ БЛОКИ ТЕСТОВ УСПЕШНО ПРОЙДЕНЫ\n" << std::endl;
    } catch (const std::exception& ErrorMessage) {
        std::cerr << "\nКРИТИЧЕСКАЯ ОШИБКА В ПРОЦЕССЕ ТЕСТИРОВАНИЯ: " << ErrorMessage.what() << std::endl;
        exit(1);
    }
}