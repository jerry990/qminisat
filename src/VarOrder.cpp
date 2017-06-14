#include "VarOrder.h"
VarOrder::VarOrder(){}

VarOrder::VarOrder(vector<lbool> * ref_to_assigns, vector<double> * ref_to_activity):
    ref_to_assigns(ref_to_assigns),
    ref_to_activity(ref_to_activity),
    activity(*ref_to_activity),
    active(set<Var>()) {}

void VarOrder::newVar(){ activity.push_back(0); }

void VarOrder::update(var x){
    active.erase(Var(x, activity[x]));
    activity[x] = (*ref_to_activity)[x];
    active.insert(Var(x, activity[x]));
}

void VarOrder::updateAll(){ for(unsigned int i = 0; i < activity.size(); i++) update(i); }

void VarOrder::undo(var x){ active.insert(Var(x, activity[x]));}

var VarOrder::select(){
    var x = 0;
    auto i = active.begin();
    while(i != active.end()){
        x = *i;
        active.erase(i++);
        if((*ref_to_assigns)[x] == l_Undef)
            return x+1;
    }
    x = 0;
    while((*ref_to_assigns)[x++] != l_Undef);
    return x;
}
