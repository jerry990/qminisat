/**************************************************************************************************
MiniSat -- Copyright (c) 2005, Niklas Sorensson
http://www.cs.chalmers.se/Cs/Research/FormalMethods/MiniSat/
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include "Solver.h"
#include <future>
#include <iostream>
typedef int var;

Solver::~Solver(){
    for(int type = 0; type < 2; ++type){
        vector<Clause*> cs = type ?
            learnts : constrs;
        for(unsigned int i = 0; i < cs.size(); ++i)
                cs[i]->remove(this);
    }
}

Solver::Solver(const Solver & s){
    while(s.nVars() > nVars()) newVar();
    for(unsigned int i = 0; i < s.constrs.size(); ++i){
        Clause* c = new Clause(*s.constrs[i]);
        constrs.push_back(c);
        watches[lit::index(~c->lits[0])].push_back(c);
        watches[lit::index(~c->lits[1])].push_back(c);
    }
    order             = VarOrder(&assigns, &activity);
    trail             = s.trail;
    assigns           = s.assigns;
    stats             = s.stats;
    cla_inc           = 1;
    var_inc           = 1;
}

Solver::Solver()
{
    order             = VarOrder(&assigns, &activity);
    cla_inc           = 1;
    var_inc           = 1;
}

var Solver::newVar(){
    int index;
    index = nVars();
    watches.push_back(vector<Clause*>());
    watches.push_back(vector<Clause*>());
    reason.push_back(NULL);
    assigns.push_back(l_Undef);
    level.push_back(-1);
    activity.push_back(0.0);
    order.newVar();
    return index;
}

Clause* Solver::propagate(){
    Clause* confl = NULL;
    while(propQ.size() > 0){
       
       stats.propagations++;
        lit p = propQ.front(); propQ.pop(); // 'p' is now the enqueued fact to propagate
        vector<Clause*>& ws = watches[lit::index(p)];
        vector<Clause*>::iterator i, j;
        for(i = j = watches[lit::index(p)].begin(); i != ws.end(); ){
            vector<lit>& lits = (*i)->lits;
            // Make sure the false literal is lits[1]:
            if(lits[0] == ~p)
                lits[0] = lits[1], lits[1] = ~p;
            // If 0th watch is true, then clause is already satisfied
            if(value(lits[0]) == l_True){
                *j++ = *i++;
                continue;
            }
            // Look for new literal to watch:
            for(unsigned int k = 2; k < lits.size(); ++k){
                if(value(lits[k]) != l_False){
                    lits[1] = lits[k], lits[k] = ~p;
                    watches[lit::index(~lits[1])].push_back(*i++); // insert clause into watcher list
                    goto next;
                }
            }
            // Clause is under unit assignment:
            *j++ = *i++;
            if(!enqueue(lits[0], *(i-1))){ // enqueue for unit propagation
                queue<lit> empty; swap(propQ, empty);
                confl = *(i-1);
                // copy remaining watches:
                while(i < ws.end())
                    *j++ = *i++;
                break;
            }
            next:;
        }
        ws.resize(ws.size() - (i-j));
    }
    return confl;
}

bool Solver::enqueue(lit &p, Clause* from){
    if(value(p) != l_Undef)
        if(value(p) == l_False)
            // Conflicting enqueued assignment
            return false;
        else
            // Existing consistent assignment -- don't enqueue
            return true;
    else{
        // New fact, store it
        assigns[lit::var(p)] = lbool(!lit::sign(p));
        level  [lit::var(p)] = decisionLevel();
        reason [lit::var(p)] = from;
        trail.push_back(p);
        propQ.push(p);
        return true;
    }
}

void Solver::analyze(Clause* confl, vector<lit> & out_learnt, int & out_btlevel){
    vector<bool> seen       = vector<bool>(nVars(), false);
    int          counter    = 0;
    lit          p          = lit_Undef;
    vector<lit>  p_reason   = vector<lit>();
    out_btlevel            = 0;
    out_learnt.push_back(lit_Undef);
    do{
        p_reason.clear();
        confl->calcReason(this, p, p_reason); // invariant here: confl != NULL
        // Trace reason for p:
        for(unsigned int j = 0; j < p_reason.size(); ++j){
            lit q = p_reason[j];
            if(!seen[lit::var(q)]){
                seen[lit::var(q)] = true;
                if(level[lit::var(q)] == decisionLevel())
                    counter++;
                else if (level[lit::var(q)] > 0){ // exclude variables from decision level 0
                    out_learnt.push_back(~q);
                    out_btlevel = max(out_btlevel, level[lit::var(q)]);
                }
            }
        }
        //select next literal to look at:
        do{
            p = trail.back();
            confl = reason[lit::var(p)];
            undoOne();
        }
        while(!seen[lit::var(p)]);
        counter--;
    }
    while(counter > 0);
    out_learnt[0] = ~p;
    stats.tot_literals += out_learnt.size();
}

void Solver::record(vector<lit> & clause){
    Clause *c = NULL; // will be set to created clause, or NULL if 'clause[]' is unit
    Clause::Clause_new(this, clause, true, false, c); // cannot fail at this point
    enqueue(clause[0], c);                     // cannot fail at this point
    if(c != NULL) learnts.push_back(c);
}

void Solver::undoOne(){
    lit p = trail.back();
    int x = lit::var(p);
    assigns[x] = l_Undef;
    reason [x] = NULL;
    level  [x] = -1;
    order.undo(x);
    trail.pop_back();
}

bool Solver::assume(lit p){
    trail_lim.push_back(trail.size());
    return enqueue(p);
}

void Solver::cancel(){
    int c = trail.size() - trail_lim.back();
    for(; c != 0; --c)
        undoOne();
    trail_lim.pop_back();
}

void Solver::cancelUntil(int level){
    while(decisionLevel() > level)
        cancel();
}

lbool Solver::search(int nof_conflicts, int nof_learnts, SearchParams& params, Cooperation * coop){
    coop->importClauses(this);
    stats.starts++;
    concurrent_queue<lit>            BUnits;
    concurrent_queue<vector<lit>>  BClauses; 
    std::thread punit[4];
   
    int conflictC = 0;
    var_decay = 1 / params.var_decay;
    cla_decay = 1 / params.cla_decay;
    model.clear();//store the result
    while(true){
        Clause* confl = propagate();
	for(int i=0; i<4; ++i){
		/*std::future<Clause*> confl*/ punit[i]=std::thread(&Solver::propagate,this); 
	}
   	for(int i=0;i<4;++i){
		punit[i].join();	
	}
        if(confl != NULL){
            // Conflict
            stats.conflicts++, conflictC++;
            vector<lit> learnt_clause = vector<lit>();
            int backtrack_level;
            if(decisionLevel() == 0)
                return l_False;
            analyze(confl, learnt_clause, backtrack_level);
            cancelUntil(max(backtrack_level, 0));
            if(decisionLevel() == 0) coop->importClauses(this);//import
            record(learnt_clause);
            coop->exportClause(this, learnt_clause);
            decayActivities();
	    //std::cout<<"  "<<coop->solvers[0]->learnts.size()<<std::endl;
        }
        else{
            // No conflict
            if(nLearnts() - nAssigns() >= nof_learnts)
                // Reduce the set of learnt clauses:
                reduceDB();
            if(nAssigns() == nVars()){
                // Model found:
                model.resize(nVars());
                for(int i = 0; i < nVars(); ++i)
                    model[i] = (value(i) == l_True);//value: return assigns[x]
                //cancelUntil(0);
                return l_True;
            }
             else if (conflictC >= nof_conflicts){
                // Reached bound on number of conflicts:
                cancelUntil(0); // force a restart
                return l_Undef;
            }
            else{
                // New variable decision:
                stats.decisions++;
                lit p = lit(order.select()); // may have heuristic for polarity here
                assume(p); // cannot return false
            }
        }
    }
}

void Solver::varBumpActivity(var x){
    if((activity[x] += var_inc) > 1e100)
        varRescaleActivity();
    order.update(x);
}

void Solver::varDecayActivity(){
    var_inc *= var_decay;
}

void Solver::varRescaleActivity(){
    for(int i = 0; i < nVars(); ++i)
        activity[i] *= 1e-100;
    var_inc *= 1e-100;
    order.updateAll();
}

void Solver::claBumpActivity(Clause* c){
    if((c->activity += cla_inc) > 1e100)
        claRescaleActivity();
}

void Solver::claDecayActivity(){
    cla_inc *= cla_decay;
}

void Solver::claRescaleActivity(){
    for(int i = 0; i < nLearnts(); ++i)
        learnts[i]->activity *= 1e-100;
    for(int i = 0; i < imports.size(); ++i)
        imports[i]->activity *= 1e-100;
     cla_inc *= 1e-100;
}

void Solver::decayActivities(){
    varDecayActivity();
    claDecayActivity();
}

void Solver::reduceDB(){
    int i, j;
    double lim = cla_inc / nLearnts();
    sort(learnts.begin(), learnts.end(), Clause::sortActivity);
    for(i=j=0; i < nLearnts()/2; ++i)
        if(!learnts[i]->locked(this))
            learnts[i]->remove(this);
        else
            learnts[j++] = learnts[i];
    for(; i < nLearnts(); ++i)
        if(!learnts[i]->locked(this)
        &&  learnts[i]->activity < lim)
            learnts[i]->remove(this);
        else
            learnts[j++] = learnts[i];
    learnts.resize(j);
}

bool Solver::simplifyDB(){
    if(propagate() != NULL)
        return false;
    for(int type = 0; type < 2; type++){
        vector<Clause*>* cs = type ?
            &learnts : &constrs;
        int j = 0;
        for(unsigned int i = 0; i < cs->size(); i++)
            if((*cs)[i]->simplify(this))
                (*cs)[i]->remove(this);
            else
                (*cs)[j++] = (*cs)[i];
        cs->resize(j);
    }
    return true;
}

bool Solver::solve(Cooperation* coop){
    SearchParams params(0.95, 0.999);
    double nof_conflicts = 100;
    double nof_learnts = nConstraints()/3.0;
    lbool status = l_Undef;
    // Solve:
    while(status == l_Undef){
        if(coop->done.load()) break;
	printf("| %9d | %7d %8d | %7d %7d %8d %7.1f |  %d  | \n",
               stats.conflicts, nConstraints(), stats.clause_literals,
               (int)nof_learnts, nLearnts(), stats.learnts_literals,
               (double)stats.learnts_literals/nLearnts(), threadId);
        status = search((int)nof_conflicts, (int)nof_learnts, params, coop);
        nof_conflicts  *= 1.1;
        nof_learnts    *= 1.1;
    }
    return status == l_True;
}

void Solver::newVar(int numVars){
    do newVar(); while(nVars() < numVars);
}

bool Solver::certify(){
    for(int i = 0; i < nConstraints(); ++i){
        bool satisfied = false;
        for(unsigned int j = 0; j < constrs[i]->lits.size(); ++j){
            lit l = constrs[i]->lits[j];
            if((lit::sign(l) && !model[lit::var(l)]) || (!lit::sign(l) && model[lit::var(l)]))
              { satisfied = true; break; }
        }
        if (!satisfied) return false;
    }
    return true;
}
