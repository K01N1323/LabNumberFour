#ifndef GENERATOR_RULES_H
#define GENERATOR_RULES_H

#include <functional>
#include "sequences/Sequence.h"

// арифм прогрессия 
template <typename T = int>
inline std::function<T(Sequence<T>*)> CreateArithmeticProgression(T start, T step) {
    return [start, step, LocalPosition = 0](Sequence<T>* self) mutable -> T {
        T NextValue = start + LocalPosition * step;
        LocalPosition++;
        return NextValue;
    };
}

// геом. прогессия 
template <typename T = int>
inline std::function<T(Sequence<T>*)> CreateGeometricProgression(T start, T multiplier) {
    return [start, multiplier, LocalPosition = 0](Sequence<T>* self) mutable -> T {
        if (LocalPosition == 0) {
            LocalPosition++;
            return start;
        }
        
        T NextValue = self->Get(LocalPosition - 1) * multiplier;
        LocalPosition++;
        return NextValue;
    };
}

// фибоначчи
template <typename T = int>
inline std::function<T(Sequence<T>*)> CreateFibonacciSequence() {
    return [LocalPosition = 0](Sequence<T>* self) mutable -> T {
        if (LocalPosition == 0) {
            LocalPosition++;
            return T(0); 
        }
        if (LocalPosition == 1) {
            LocalPosition++;
            return T(1);
        }
        
        T NextValue = self->Get(LocalPosition - 1) + self->Get(LocalPosition - 2);
        LocalPosition++;
        return NextValue;
    };
}

// простые числа
template <typename T = int>
inline std::function<T(Sequence<T>*)> CreatePrimeSequence() {
    return [CurrentCandidate = T(2), LocalPosition = 0](Sequence<T>* self) mutable -> T {
        while (true) {
            bool IsPrime = true;
            
            for (int CheckIndex = 0; CheckIndex < LocalPosition; CheckIndex++) {
                T PreviousPrime = self->Get(CheckIndex);
                
                if (PreviousPrime * PreviousPrime > CurrentCandidate) {
                    break; 
                }
                
                if (CurrentCandidate % PreviousPrime == T(0)) {
                    IsPrime = false;
                    break;
                }
            }
            
            if (IsPrime) {
                T FoundPrime = CurrentCandidate;
                CurrentCandidate++;
                LocalPosition++;
                return FoundPrime;
            }
            CurrentCandidate++;
        }
    };
}

#endif // GENERATOR_RULES_H