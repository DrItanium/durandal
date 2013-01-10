#include "Passes/JSEdgeRemoval.h"

char JSEdgeRemoval::ID = 0;
static RegisterPass<JSEdgeRemoval> js("jsedge", "JS Edge Removal", false, false);
