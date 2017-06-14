#ifndef VAR_H
#define VAR_H
typedef int var;
class Var
{
public:
    var value;
    double activity;
    Var(var value): value(value), activity(0) {}
    Var(var value, double activity): value(value), activity(activity) {};
    operator int() const { return value; };
    bool operator < (const Var& rhs) const { return activity > rhs.activity; };
};

static const var var_Undef = -1;

#endif // VAR_H
