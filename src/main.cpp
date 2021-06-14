#include <iostream>

#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/RboxPoints.h>

#include "graph/ExplicitCouplingGraph.h"
#include "graph/SimilarityCouplingGraph.h"
#include "graph/ModuleDistanceCouplingGraph.h"
#include "graph/WeightCombinedGraph.h"
#include "graph/ResultCachedGraph.h"
#include "util.h"
#include "DataStorage.h"
#include "CommandExecutor.h"
#include "BestResultSet.h"


using namespace std;


void mainLoop() {
    CommandExecutor executor;
    string line;
    while (getline(cin, line)) {
        executor.tryExecuteCommand(line);
    }
}

int main() {
    cout.precision(10);
    cout << fixed;
    mainLoop();

    // BestResultSet<int> brs(4, 100);
    // brs.trim();

    return 0;
}
