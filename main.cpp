#include <iostream>

#include "graph/ExplicitCouplingGraph.h"
#include "graph/SimilarityCouplingGraph.h"
#include "graph/ModuleDistanceCouplingGraph.h"
#include "graph/WeightCombinedGraph.h"
#include "graph/ResultCachedGraph.h"
#include "util.h"
#include "GraphStorage.h"
#include "CommandExecutor.h"

using namespace std;


void mainLoop() {
    CommandExecutor executor;
    string line;
    while (getline(cin, line)) {
        executor.tryExecuteCommand(line);
    }
}

int main(int argc, const char* argv[]) {
    mainLoop();
    cout << "Hello, World!" << endl;

    ExplicitCouplingGraph g("foo");
    SimilarityCouplingGraph g2("bar");
    ModuleDistanceCouplingGraph g3;

    return 0;
}
