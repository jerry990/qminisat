#ifndef COOPERATION_H
#define COOPERATION_H
#include <cassert>
#include <atomic>
#include <thread>
#include <set>
#include <vector>
#include <iostream>
#include "tbb/tbb.h"
#include <Solver.h>
#include <lit.h>
#include <Clause.h>
#include <SolverStats.h>
#include <SearchParams.h>
#include <queue>
#define  NBTHREADS 1
using namespace tbb;
using namespace std;

class Clause;
class Solver;
class Cooperation
{
    std::thread                    threads[NBTHREADS];
    concurrent_queue<lit>            Units[NBTHREADS];
    concurrent_queue<vector<lit>>  Clauses[NBTHREADS];
    static void      solve(Cooperation* coop, int id);
public:
    Solver*                        solvers[NBTHREADS];
    void                                 copy_solve();
    void                     importClauses(Solver* s);
    void     exportClause(Solver* s, vector<lit>& ps);
    std::atomic<bool>                            done;
    Cooperation();
   ~Cooperation();
};

#endif // COOPERATION_H
