#ifndef LIT_H
#define LIT_H
#include <cmath>
class lit
{
    int x;
public:
    lit(): x(0) {}
    lit(int x): x(x) {}
    static int var(lit p){ return std::abs(p.x) -1; }
    static bool sign(lit p){ return p.x<0; }
    static int index(lit p){ return var(p)*2 + (sign(p) ? 1: 0); }
    lit operator~(){ return lit(-x); }
    bool operator==(const lit rhs) const{
        return var(*this) == var(rhs) && sign(*this) == sign(rhs);
    }
    bool operator!=(const lit rhs) const{
        return var(*this) != var(rhs) || sign(*this) != sign(rhs);
    }
};

static lit lit_Undef = lit();

#endif // LIT_H
