#include "Cooperation.h"

Cooperation::Cooperation(){
    solvers[0] = new Solver();
}
Cooperation::~Cooperation(){
    for(int t = 0; t < NBTHREADS; ++t)//Cooperation.h:NBTHREADS	
        delete solvers[t];
}
void Cooperation::copy_solve(){
    for(int t = 0; t < NBTHREADS; ++t){//for_mod
        if(t != 0) solvers[t] = new Solver(*solvers[0]);
        solvers[t]->threadId = t;
    }
    for(int t = 0; t < NBTHREADS; ++t)
        threads[t] = std::thread(solve, this, t);
    for(int t = 0; t < NBTHREADS; ++t)
        threads[t].join();
}
void Cooperation::solve(Cooperation* coop, int id){
    lbool st = coop->solvers[id]->solve(coop);
    bool fals = false;
    if(coop->done.compare_exchange_strong(fals, true)){
        coop->solvers[id]->stats.printStats(coop->solvers[id]->stats, NBTHREADS,id);
        cout << (st == l_True ? assert(coop->solvers[id]->certify()),"\nSATISFIABLE" : "\nUNSATISFIABLE") << endl;
    }
	//std::cout<<coop->solvers[id]->nAssigns()<<"    id: "<<id<<std::endl;
	
	//for(int i=0; i < coop->solvers[id]->trail.size();++i){
	//	std::cout<<coop->solvers[id]->trail[i]<<std::endl;
	//}
}

void Cooperation::importClauses(Solver* s){
    vector<lit> ps;
    set<int> hashes;
    while(Clauses[s->threadId].try_pop(ps)){
        int hash = 0;
        for(int i = 0; i < ps.size(); ++i)
            hash += lit::index(ps[i]); 
        if(!(hashes.insert(hash)).second) continue;
        Clause* c = NULL;
        Clause::Clause_new(s, ps, true, true, c);
        if(c != NULL) s->imports.push_back(c);
    }   
}

void Cooperation::exportClause(Solver* s, vector<lit> & ps){
    if(ps.size() > 10) return; 
    for(int t = 0; t < NBTHREADS; ++t){
        if(t == s->threadId) continue;
        Clauses[t].push(ps);
    }
}


