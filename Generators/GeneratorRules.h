#ifndef GENERATOR_RULES_H
#define GENERATOR_RULES_H

#include <functional>
#include "Sequence.h"

// 1. Арифметическая прогрессия (Оставляем формулу, так как это O(1) и без рекурсии)
inline std::function<int(Sequence<int>*)> CreateArithmeticProgression(int start, int step) {
    return [start, step, LocalPosition = 0](Sequence<int>* self) mutable -> int {
        int NextValue = start + LocalPosition * step;
        LocalPosition++;
        return NextValue;
    };
}

// 2. Геометрическая прогрессия (Используем рекуррентное обращение к self)
inline std::function<int(Sequence<int>*)> CreateGeometricProgression(int start, int multiplier) {
    return [start, multiplier, LocalPosition = 0](Sequence<int>* self) mutable -> int {
        if (LocalPosition == 0) {
            LocalPosition++;
            return start;
        }
        
        // Берем предыдущий элемент из кэша и умножаем
        int NextValue = self->Get(LocalPosition - 1) * multiplier;
        LocalPosition++;
        return NextValue;
    };
}

// 3. Числа Фибоначчи (Идеальный пример для мемоизации!)
inline std::function<int(Sequence<int>*)> CreateFibonacciSequence() {
    return [LocalPosition = 0](Sequence<int>* self) mutable -> int {
        if (LocalPosition == 0) {
            LocalPosition++;
            return 0;
        }
        if (LocalPosition == 1) {
            LocalPosition++;
            return 1;
        }
        
        // Берем уже вычисленные и закэшированные значения из самой последовательности
        int NextValue = self->Get(LocalPosition - 1) + self->Get(LocalPosition - 2);
        LocalPosition++;
        return NextValue;
    };
}

// 4. Простые числа (Оптимизация: делим только на уже найденные простые числа)
inline std::function<int(Sequence<int>*)> CreatePrimeSequence() {
    return [CurrentCandidate = 2, LocalPosition = 0](Sequence<int>* self) mutable -> int {
        while (true) {
            bool IsPrime = true;
            
            // Проверяем делимость только на ранее сгенерированные простые числа
            for (int CheckIndex = 0; CheckIndex < LocalPosition; CheckIndex++) {
                int PreviousPrime = self->Get(CheckIndex);
                
                // Оптимизация: проверяем только до корня из CurrentCandidate
                if (PreviousPrime * PreviousPrime > CurrentCandidate) {
                    break; 
                }
                
                if (CurrentCandidate % PreviousPrime == 0) {
                    IsPrime = false;
                    break;
                }
            }
            
            if (IsPrime) {
                int FoundPrime = CurrentCandidate;
                CurrentCandidate++;
                LocalPosition++;
                return FoundPrime;
            }
            CurrentCandidate++;
        }
    };
}

#endif // GENERATOR_RULES_H