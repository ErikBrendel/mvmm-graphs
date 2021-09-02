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
        if (steps_a[i] != steps_b[i]) {
            // unequal: calc distance
            // one is subtracted to allow a "direct step" from one node in the tree to its sibling
            return totalLength - (int) (i * 2) - 1;
        }
    }
    return totalLength - (minLen * 2);
}

ModuleDistanceCouplingGraph::ModuleDistanceCouplingGraph() : CouplingGraph("module_distance") {}

double ModuleDistanceCouplingGraph::getNormalizedSupport(const string& node) {
    return 1;
}

double ModuleDistanceCouplingGraph::getNormalizedCoupling(const string& a, const string& b) {
    int dist = path_module_distance(a, b);
    // one is subtracted from the dist, so that the lowest possible dist (1) a perfect coupling score
    if (dist > 0) dist--;
    double base = 1.1f;
    return (double) pow(base, -dist);
}

void ModuleDistanceCouplingGraph::printStatistics() {
    cout << "Module Distance" << endl;
}
