#ifndef DIMACS_H
#define DIMACS_H
#include <stdio.h>
#include <Solver.h>
class Dimacs
{
public:
    char* readFile(FILE* in);
    inline void skipWhitespace(char** in);
    inline void skipLine(char** in);
    inline void skipLetters(char** in);
    inline int parseInt(char** in);
    void readClause(char** in, Solver* s, vector<lit>& lits);
    lbool parse_DIMACS_main(char* in, Solver* s);
    lbool parse_DIMACS(FILE * in, Solver* s);
};
#endif // DIMACS_H
