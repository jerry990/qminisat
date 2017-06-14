#ifndef VARORDER_H
#define VARORDER_H
#include <lbool.h>
#include <vector>
#include <set>
#include <Var.h>
#include <algorithm>
#include <lit.h>

typedef int var;
using namespace std;
class VarOrder
{
    vector<lbool> *            ref_to_assigns;
    vector<double> *           ref_to_activity;
    vector<double>             activity;
    set<Var>                   active;            // set containing active unselected variables
public:
    VarOrder();
    VarOrder(vector<lbool> * ref_to_assigns, vector<double> * ref_to_activity);
    void newVar();             // Called when a new variable is created.
    void update(var x);        // Called when variable has increased in activity.
    void updateAll();          // Called when all variables have been assigned new activities.
    void undo(var x);          // Called when variable is unbound (may be selected again).
    var  select();             // Called to select a new, unassigned variable.
};

#endif // VARORDER_H
