#ifndef LBOOL_H
#define LBOOL_H
class lbool
{
public:
    bool defined;
    bool value;
    lbool(): defined(false) {}
    lbool(bool value): defined(true), value(value) {}
    lbool operator ~(){ if(!defined) return lbool(); else return lbool(!value); }
    bool operator ==(const lbool rhs) const {
        return (!defined && !rhs.defined) ||
               ( defined &&  rhs.defined && value == rhs.value);
    }
    bool operator !=(const lbool rhs) const { return !(*this == rhs); }
};

const static lbool l_False = lbool(false);
const static lbool l_True  = lbool(true);
const static lbool l_Undef = lbool();

#endif // LBOOL_H
