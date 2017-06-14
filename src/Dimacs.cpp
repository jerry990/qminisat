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

#include "Dimacs.h"

char* Dimacs::readFile(FILE * in){
    char*   data = (char*)malloc(65536);
    int     cap  = 65536;
    int     size = 0;

    while (!feof(in)){
        if (size == cap){
            cap *= 2;
            data = (char*)realloc(data, cap); }
        size += fread(&data[size], 1, 65536, in);
    }
    data = (char*)realloc(data, size+1);
    data[size] = '\0';

    return data;
}

inline void Dimacs::skipWhitespace(char** in) {
    while ((**in >= 9 && **in <= 13) || **in == 32)
        (*in)++; }

inline void Dimacs::skipLetters(char** in) {
    while (**in >= 97 && **in <= 122)
        (*in)++;
}

inline void Dimacs::skipLine(char** in) {
    for (;;){
        if (**in == 0) return;
        if (**in == '\n') { (*in)++; return; }
        (*in)++; } }

inline int Dimacs::parseInt(char** in) {
    int     val = 0;
    int    _neg = 0;
    skipWhitespace(in);
    if      (**in == '-') _neg = 1, (*in)++;
    else if (**in == '+') (*in)++;
    if (**in < '0' || **in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", **in), exit(1);
    while (**in >= '0' && **in <= '9')
        val = val*10 + (**in - '0'),
        (*in)++;
    return _neg ? -val : val; }

void Dimacs::readClause(char** in, Solver* s, vector<lit>& lits) {
    int parsed_lit, var;
    for (;;){
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        while(var >= s->nVars()) s->newVar();
        lits.push_back(lit(parsed_lit));
    }
}

lbool Dimacs::parse_DIMACS_main(char* in, Solver* s) {
    for (;;){
        skipWhitespace(&in);
        if (*in == 0)
            break;
        else if (*in == 'c' || *in == 'p')
            skipLine(&in);
        else{
            vector<lit> lits = vector<lit>();
            readClause(&in, s, lits);
            Clause* c = NULL;
            if(!Clause::Clause_new(s, lits, false, false, c))
                return l_False;
            if(c != NULL) s->constrs.push_back(c);
        }
    }
    return s->simplifyDB();
}


// Inserts problem into solver. Returns FALSE upon immediate conflict.
//
lbool Dimacs::parse_DIMACS(FILE * in, Solver* s) {
    char* text = readFile(in);
    lbool ret  = parse_DIMACS_main(text, s);
    free(text);
    return ret; }
