#include <iostream>

#include "graph/ExplicitCouplingGraph.h"
#include "graph/SimilarityCouplingGraph.h"
#include "graph/ModuleDistanceCouplingGraph.h"
#include "graph/WeightCombinedGraph.h"
#include "graph/ResultCachedGraph.h"
#include "util.h"

using namespace std;

int main() {
    cout << "Hello, World!" << endl;

    ExplicitCouplingGraph g("foo");
    SimilarityCouplingGraph g2("bar");
    ModuleDistanceCouplingGraph g3;

    return 0;
}
