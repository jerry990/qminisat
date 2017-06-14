#ifndef CLAUSE_H
#define CLAUSE_H
#include <Solver.h>
#include <vector>
#include <lit.h>
using namespace std;
class Solver;
class Clause
{
public:
    bool         learnt;
    bool         imported;
    float        activity;
    vector<lit>  lits;
    static bool  Clause_new(Solver* S, vector<lit> & ps, bool learnt, bool imported, Clause* & c);
    bool         locked(Solver* S);
    void         remove(Solver* S);
    bool         simplify(Solver* S);
    void         calcReason(Solver* S, lit p, vector<lit> & out_reason);
    void         removeElem(Clause* c, vector<Clause*> & v);
    static bool  sortActivity(Clause* l, Clause* r){ return l->activity > r->activity;}
};
#endif // CLAUSE_H
