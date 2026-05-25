#ifndef GENERATOR_RULES_H
#define GENERATOR_RULES_H

#include <functional>
#include "sequences/Sequence.h"

// арифм прогрессия 
inline std::function<int(Sequence<int>*)> CreateArithmeticProgression(int start, int step) {
    return [start, step, LocalPosition = 0](Sequence<int>* self) mutable -> int {
        int NextValue = start + LocalPosition * step;
        LocalPosition++;
        return NextValue;
    };
}

// геом. прогессия 
inline std::function<int(Sequence<int>*)> CreateGeometricProgression(int start, int multiplier) {
    return [start, multiplier, LocalPosition = 0](Sequence<int>* self) mutable -> int {
        if (LocalPosition == 0) {
            LocalPosition++;
            return start;
        }
        
        int NextValue = self->Get(LocalPosition - 1) * multiplier;
        LocalPosition++;
        return NextValue;
    };
}

// фибоначчи
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
        
        int NextValue = self->Get(LocalPosition - 1) + self->Get(LocalPosition - 2);
        LocalPosition++;
        return NextValue;
    };
}

// простые числа
inline std::function<int(Sequence<int>*)> CreatePrimeSequence() {
    return [CurrentCandidate = 2, LocalPosition = 0](Sequence<int>* self) mutable -> int {
        while (true) {
            bool IsPrime = true;
            
            // проверка только на ранее полученных числах
            for (int CheckIndex = 0; CheckIndex < LocalPosition; CheckIndex++) {
                int PreviousPrime = self->Get(CheckIndex);
                
     
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