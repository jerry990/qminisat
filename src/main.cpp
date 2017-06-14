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

#include <iostream>
#include <Solver.h>
#include <Dimacs.h>
#include <cassert>
#include <Cooperation.h>

Cooperation coop;

int main(int argc, char *argv[])
{
    FILE* in;
    Dimacs parser = Dimacs();
    lbool st;//lbool.defined=false
    if(argc != 2){
        cerr << "ERROR! Not enough command line arguments" << endl;
        exit(1);
    }
    in = fopen(argv[1], "rb");
    if(in == NULL){
        cerr << "ERROR! Could not open file: " << argv[1] << endl;
        exit(1);
    }
    st = parser.parse_DIMACS(in, coop.solvers[0]);
    fclose(in);
    if(st == l_False){
        cout << "Trivial problem\nUNSATISFIABLE\n" << endl;
        exit(20);
    }
    printf("===============================[MINISAT]=================================\n");
    printf("| Conflicts |     ORIGINAL     |              LEARNT              | TID |\n");
    printf("|           | Clauses Literals |   Limit Clauses Literals  Lit/Cl |     |\n");
    printf("=========================================================================\n");
    coop.copy_solve();//cooperation.cpp
    return 0;
}
