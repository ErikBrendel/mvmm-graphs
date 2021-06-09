#pragma once

#include "CouplingGraph.h"

int path_module_distance(const string& a, const string& b);

class ModuleDistanceCouplingGraph : public CouplingGraph {
public:
    ModuleDistanceCouplingGraph();

    float getNormalizedSupport(const string &node) override;
    float getNormalizedCoupling(const string &a, const string &b) override;
    void plaintextContent(ostream& out) override;
    void printStatistics() override;
};


