#include <iostream>

#include "ExplicitCouplingGraph.h"
#include "SimilarityCouplingGraph.h"
#include "ModuleDistanceCouplingGraph.h"
#include "WeightCombinedGraph.h"
#include "ResultCachedGraph.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    ExplicitCouplingGraph g("foo");
    SimilarityCouplingGraph g2("bar");
    ModuleDistanceCouplingGraph g3;
    

    return 0;
}
