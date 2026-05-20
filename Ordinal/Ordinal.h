#ifndef ORDINAL_H
#define ORDINAL_H

struct Ordinal {
    int omega; 
    int index; 

    Ordinal(int OmegaValue, int IndexValue) : omega(OmegaValue), index(IndexValue) {}
    
    Ordinal(int IndexValue) : omega(0), index(IndexValue) {}
    
    Ordinal() : omega(0), index(0) {}

    // Если omega больше нуля, считаем последовательность бесконечной
    bool IsInfinite() const {
        return omega > 0;
    }

    // Получить конечное количество элементов (если применимо)
    int GetCount() const {
        return index;
    }

    // Статический генератор бесконечного ординала
    static Ordinal Infinite() {
        return Ordinal(1, 0);
    }
};

#endif // ORDINAL_H