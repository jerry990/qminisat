#ifndef SOLVERSTATS_H
#define SOLVERSTATS_H
#include <ctime>
#include <iostream>
#include "Cooperation.h"
class SolverStats
{
public:
    int starts, decisions, propagations, conflicts;
    int clause_literals, learnts_literals, tot_literals;
    SolverStats() : starts(0), decisions(0), propagations(0), conflicts(0)
      , clause_literals(0), learnts_literals(0), tot_literals(0) {};
    static void printStats(SolverStats& stats, int nbthreads,int & id)
    {
        double  cpu_time = (double)clock()/CLOCKS_PER_SEC;
        printf("=========================================================================\n");
        printf("restarts              : %d\n", stats.starts);
        printf("conflicts             : %-12d   (%.0f /sec)\n", stats.conflicts   , stats.conflicts   /cpu_time);
        printf("decisions             : %-12d   (%.0f /sec)\n", stats.decisions   , stats.decisions   /cpu_time);
        printf("propagations          : %-12d   (%.0f /sec)\n", stats.propagations, stats.propagations/cpu_time);
        printf("conflict literals     : %-12d              \n", stats.tot_literals                             );
        printf("CPU time              : %g s\n", cpu_time/nbthreads);
	printf("id_num		      : %d \n",id);
	
	
    }
};


#endif // SOLVERSTATS_H
