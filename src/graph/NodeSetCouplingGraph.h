#pragma once

#include <unordered_map>
#include <unordered_set>

#include "CouplingGraph.h"

class NodeSetCouplingGraph : public CouplingGraph {
private:
    unordered_map<string, unordered_set<string>> children;
    bool childrenDictCouldBeOutdated;

public:
    explicit NodeSetCouplingGraph(const string& name);

    const vector<string>& getNodeSet() override = 0;

    virtual const unordered_set<string>& getChildren(const string& node);
    virtual vector<string> getSelfAndDescendants(const string& node);

protected:
    void onNodeSetChanged();

private:
    void createChildCache();
    void getSelfAndDescendentsRec(const string& node, vector<string>& result);
};


