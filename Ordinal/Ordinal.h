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

    
    bool operator==(const Ordinal& other) const {
        return omega == other.omega && index == other.index;
    }

    bool operator!=(const Ordinal& other) const {
        return !(*this == other);
    }

    bool operator<(const Ordinal& other) const {
        if (omega != other.omega) {
            return omega < other.omega; 
        }
        return index < other.index;    
    }

    bool operator>(const Ordinal& other) const { 
        return other < *this; 
    }

    bool operator<=(const Ordinal& other) const { 
        return !(other < *this); 
    }

    bool operator>=(const Ordinal& other) const { 
        return !(*this < other); 
    }



    Ordinal operator+(const Ordinal& other) const {
        if (other.omega > 0) {
      
            return Ordinal(this->omega + other.omega, other.index);
        } else {

            return Ordinal(this->omega, this->index + other.index);
        }
    }
    
    Ordinal& operator+=(const Ordinal& other) {
        *this = *this + other;
        return *this;
    }
};

#endif // ORDINAL_H