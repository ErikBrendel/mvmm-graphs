#pragma once

#include "CouplingGraph.h"

int path_module_distance(const string& a, const string& b);

class ModuleDistanceCouplingGraph : public CouplingGraph {
public:
    ModuleDistanceCouplingGraph();

    double getNormalizedSupport(const string &node) override;
    double getNormalizedCoupling(const string &a, const string &b) override;
    void printStatistics() override;
};


