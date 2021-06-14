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
    // mainLoop();

    orgQhull::Qhull qhull;
    orgQhull::RboxPoints points("20 D2");
    qhull.runQhull(points, "");
    auto vertices = qhull.vertexList();
    for (auto it = vertices.constBegin(); it != vertices.constEnd(); ++it) {
        const auto& vert = it->point();
        vector<double> p(vert.dimension());
        rep(d, vert.dimension()) {
            p.push_back(vert[d]);
        }
        cout << p << endl;
    }


    return 0;
}
