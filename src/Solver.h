#ifndef SOLVER_H
#define SOLVER_H
#include <vector>
#include <queue>
#include <Clause.h>
#include <math.h>
#include <SearchParams.h>
#include <Cooperation.h>
#include <VarOrder.h>
#include <lit.h>
#include <lbool.h>
#include <SolverStats.h>

using namespace std;
class Cooperation;
class Clause;
class Solver
{
public:
    /* Internal state of the solver */
    vector<Clause*>      constrs;           // List of problem constraints.
    vector<Clause*>      learnts;           // List of learnt clauses.
    vector<Clause*>      imports;           // List of imported learnt clauses.
    double               cla_inc;           // Clause activity increment - amount to bump with.
    double               cla_decay;         // Decay factor for clause activity.
    // Variable order
    vector<double>       activity;          // Heuristic measurement of the activity of a variable.
    double               var_inc;           // Variable activity increment - amount to bump with.
    double               var_decay;         // Decay factor for variable activity.
    VarOrder             order;             // Keeps track of the dynamic variable order.
    // Propagation
    vector<vector<Clause*>>                 // For each literal 'p', a list of Constraints watching 'p'.
                         watches;           // A Constraint will be inspected when 'p' becomes true.
    queue<lit>           propQ;             // Propagation queue.
    // Assignments
    vector<lbool>        assigns;           // The current assignments indexed on variables.
    vector<lit>          trail;             // List of assignments in chronological order.
    vector<int>          trail_lim;         // Seperator indices for different decision levels in 'trail'.
    vector<Clause*>      reason;            // For each variable, the constraint that implied its value.
    vector<int>          level;             // For each variable, the decision level it was assigned.
    vector<bool>         model;             // store the results.
    SolverStats          stats;
    int                  threadId;

    // Small helper methods
    int   nVars()    const { return assigns.size(); }
    int   nAssigns() const { return trail.size(); }
    int   nConstraints()   { return constrs.size(); }
    int   nLearnts()       { return learnts.size(); }
    lbool value(int x)     { return assigns[x]; }
    lbool value(lit p)     { return lit::sign(p) ? ~assigns[lit::var(p)] : assigns[lit::var(p)]; }
    int   decisionLevel()  { return trail_lim.size(); }

    Solver();
    Solver(const Solver &);
    ~Solver();
    int     newVar();
    Clause* propagate();
    bool    enqueue(lit& p, Clause* from = NULL);
    void    analyze(Clause * confl, vector<lit> & out_learnt, int & out_btlevel);
    void    record(vector<lit> & clause);
    void    undoOne();
    bool    assume(lit p);
    void    cancel();
    void    cancelUntil(int level);
    lbool   search(int nof_conflicts, int nof_learnts, SearchParams& params, Cooperation* coop);
    void    varBumpActivity(int x);
    void    varDecayActivity();
    void    varRescaleActivity();
    void    claBumpActivity(Clause * c);
    void    claDecayActivity();
    void    claRescaleActivity();
    void    decayActivities();
    void    reduceDB();
    bool    simplifyDB();
    bool    solve(Cooperation* coop);
    void    newVar(int numVars);
    bool    certify();
};

#endif // SOLVER_H
