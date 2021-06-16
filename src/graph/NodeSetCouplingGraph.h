#pragma once

#include <unordered_map>
#include <unordered_set>

#include "CouplingGraph.h"

class NodeSetCouplingGraph : public CouplingGraph {
private:
    unordered_map<string, unordered_set<string>> children;

public:
    explicit NodeSetCouplingGraph(const string& name);

    const vector<string>& getNodeSet() override = 0;

    virtual const unordered_set<string>& getChildren(const string& node);

private:
    void createChildCache();
};


