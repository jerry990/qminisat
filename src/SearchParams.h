#ifndef SEARCHPARAMS_H
#define SEARCHPARAMS_H
class SearchParams
{
public:
    double var_decay;
    double cla_decay;
    SearchParams(double var_decay, double cla_decay):
        var_decay(var_decay), cla_decay(cla_decay) {}
};

#endif // SEARCHPARAMS_H
