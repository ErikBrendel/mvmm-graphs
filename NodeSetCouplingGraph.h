#pragma once

#include <unordered_map>
#include "CouplingGraph.h"

class NodeSetCouplingGraph : public CouplingGraph {
private:
    unordered_map<string, vector<string>> children;

public:
    explicit NodeSetCouplingGraph(const string& name);

    const vector<string>& getNodeSet() override = 0;

    vector<string> getChildren(const string& node);

private:
    void createChildCache();
};


