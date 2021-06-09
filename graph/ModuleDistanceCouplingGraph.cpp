#include "ModuleDistanceCouplingGraph.h"

#include <cmath>
#include <iostream>

#include "../util.h"

int path_module_distance(const string& a, const string& b) {
    if (a == b) return 0;
    auto steps_a = split(a, "/");
    auto steps_b = split(b, "/");
    int minLen = (int)min(steps_a.size(), steps_b.size());
    int totalLength = (int)(steps_a.size() + steps_b.size());
    rep(i, minLen) {
        if (steps_a[i] == steps_b[i]) continue;
        //unequal: calc distance
        return totalLength - (int)(i * 2);
    }
    return totalLength - (minLen * 2);
}

ModuleDistanceCouplingGraph::ModuleDistanceCouplingGraph() : CouplingGraph("module_distance") {}

float ModuleDistanceCouplingGraph::getNormalizedSupport(const string& node) {
    return 1;
}

float ModuleDistanceCouplingGraph::getNormalizedCoupling(const string& a, const string& b) {
    int dist = path_module_distance(a, b);
    float base = 1.1f;
    return (float) pow(base, -dist);
}

void ModuleDistanceCouplingGraph::printStatistics() {
    cout << "Module Distance" << endl;
}
