#include "NodeSetCouplingGraph.h"

#include <iostream>

#include "../util.h"

using namespace std;

NodeSetCouplingGraph::NodeSetCouplingGraph(const string& name)
        : CouplingGraph(name), children(), childrenDictCouldBeOutdated{true} {}

void NodeSetCouplingGraph::createChildCache() {
    if (!children.empty()) {
        cout << "re-calculating children dict!" << endl;
    }
    for (const auto& n: getNodeSet()) {
        string current = n;
        children[n];
        while (!current.empty()) {
            string p = getParent(current);
            if (!children[p].insert(current).second) break;
            current = p;
        }
    }
    childrenDictCouldBeOutdated = false;
}

unordered_set<string> noChildren;

const unordered_set<string>& NodeSetCouplingGraph::getChildren(const string& node) {
    if (childrenDictCouldBeOutdated) {
        createChildCache();
    }
    auto found = children.find(node);
    if (found == children.end()) {
        return noChildren;
    }
    return found->second;
}

void NodeSetCouplingGraph::onNodeSetChanged() {
    childrenDictCouldBeOutdated = true;
}
