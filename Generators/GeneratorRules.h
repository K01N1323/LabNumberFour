#ifndef GENERATOR_RULES_H
#define GENERATOR_RULES_H

#include <functional>
#include "Sequence.h"

//  Арифметическая прогрессия
inline std::function<int(Sequence<int>*)> CreateArithmeticProgression(int start, int step) {
    return [start, step, LocalPos = 0](Sequence<int>* self) mutable -> int {
        int value = start + LocalPos * step;
        LocalPos++;
        return value;
    };
}

// Геометрическая прогрессия
inline std::function<int(Sequence<int>*)> CreateGeometricProgression(int start, int multiplier) {
    return [CurrentValue = start, multiplier](Sequence<int>* self) mutable -> int {
        int value = CurrentValue;
        CurrentValue *= multiplier;
        return value;
    };
}

//  Числа Фибоначчи
inline std::function<int(Sequence<int>*)> CreateFibonacciSequence() {
    return [PrevFirst = 0, PrevSecond = 1, LocalPos = 0](Sequence<int>* self) mutable -> int {
        if (LocalPos == 0) {
            LocalPos++;
            return PrevFirst;
        }
        if (LocalPos == 1) {
            LocalPos++;
            return PrevSecond;
        }
        
        int NextValue = PrevFirst + PrevSecond;
        PrevFirst = PrevSecond;
        PrevSecond = NextValue;
        
        LocalPos++;
        return NextValue;
    };
}

// 4. Простые числа
inline std::function<int(Sequence<int>*)> CreatePrimeSequence() {
    return [current = 2](Sequence<int>* self) mutable -> int {
        auto IsPrime = [](int num) -> bool {
            if (num < 2) return false;
            for (int index = 2; index * index <= num; index++) {
                if (num % index == 0) return false;
            }
            return true;
        };

        while (!IsPrime(current)) {
            current++;
        }
        
        int value = current;
        current++;
        return value;
    };
}

#endif // GENERATOR_RULES_H