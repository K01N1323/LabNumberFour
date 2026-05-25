#ifndef ORDINAL_H
#define ORDINAL_H

struct Ordinal {
    int omega; 
    int index; 

    Ordinal(int OmegaValue, int IndexValue) : omega(OmegaValue), index(IndexValue) {}
    
    Ordinal(int IndexValue) : omega(0), index(IndexValue) {}
    
    Ordinal() : omega(0), index(0) {}

    bool IsInfinite() const {
        return omega > 0;
    }

    int GetCount() const {
        return index;
    }

    static Ordinal Infinite() {
        return Ordinal(1, 0);
    }
};

#endif // ORDINAL_H