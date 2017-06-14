#include "Clause.h"

bool Clause::Clause_new(Solver* S, vector<lit> & ps, bool learnt, bool imported, Clause* & c){
    // Normalize clause:
    if(!learnt){
        lit p = lit_Undef; unsigned int i, j;
        for(i = j = 0; i < ps.size(); ++i)
            if(S->value(ps[i]) == l_True || ps[i] == ~p)
                return true;
            else if(S->value(ps[i]) != l_False && ps[i] != p)
                ps[j++] = p = ps[i];
        ps.resize(j);
    }
    if(ps.size() == 0)
        return false;
    else if (ps.size() == 1)
        return S->enqueue(ps[0]); // unit facts are enqueued
    else{
        // Allocate clause:
        c = new Clause();
        c->lits = ps;
        c->learnt = learnt;
        c->imported = imported;
        c->activity = 0; // only relevant for learnt clauses
        if(learnt){
            // Pick a second literal to watch;
            // Let max_i be the index of the literal with highest decision level
            int max_i = 1;
            for(unsigned int i = 2; i < ps.size(); i++)
                if(S->level[lit::var(ps[i])] > S->level[lit::var(ps[max_i])])
                    max_i = i;
            c->lits[1] = ps[max_i]; c->lits[max_i] = ps[1];
            // Bumping:
            S->claBumpActivity(c); // newly learnt clauses should be considered active
            for(unsigned int i = 0; i < ps.size(); i++)
               S->varBumpActivity(lit::var(ps[i])); // variables in conflict are bumped
            S->stats.learnts_literals += c->lits.size();
        }
        else
            S->stats.clause_literals += c->lits.size();
        // Add clause to watcher lists:
        S->watches[lit::index(~c->lits[0])].push_back(c);
        S->watches[lit::index(~c->lits[1])].push_back(c);
        return true;
    }
}

bool Clause::locked(Solver* S){
    return this == S->reason[lit::var(lits[0])];
}

// Constraint interface:
void Clause::remove(Solver* S){
    if(this->learnt) S->stats.learnts_literals -= this->lits.size();
    else             S->stats.clause_literals -= this->lits.size();
    removeElem(this, S->watches[lit::index(~lits[0])]);
    removeElem(this, S->watches[lit::index(~lits[1])]);
    delete this;
}

void Clause::removeElem(Clause* c, vector<Clause*> & v){
    v.erase(std::remove(v.begin(), v.end(), c), v.end());
}

bool Clause::simplify(Solver* S){ // only called at top-level with empty prop. queue
    int j = 0;
    for(unsigned int i = 0; i < lits.size(); i++)
        if(S->value(lits[i]) == l_True)
            return true;
        else if(S->value(lits[i]) == l_Undef)
            lits[j++] = lits[i]; // false literals are not copied (only occur for i >= 2)
    lits.resize(j);
    return false;
}

void Clause::calcReason(Solver* S, lit p, vector<lit> & out_reason){
    // invariant: (p == lit_Undef or (p == lits[0])
    for(unsigned int i = ((lit_Undef == p) ? 0 : 1); i < lits.size(); i++)
        out_reason.push_back(~lits[i]); // invariant: S.value(lits[i]) == FALSE
    if(learnt) S->claBumpActivity(this);
}
