//
// Created by erik on 08.06.21.
//

#include "NodeSetCouplingGraph.h"

#include "iostream"
#include "util.h"

using namespace std;

NodeSetCouplingGraph::NodeSetCouplingGraph(const string& name)
        : CouplingGraph(name), children() {}

void NodeSetCouplingGraph::createChildCache() {
    if (!children.empty()) {
        cout << "re-calculating children dict!" << endl;
    }
    for (const auto& n: getNodeSet()) {
        string current = n;
        children[n];
        while (!current.empty()) {
            string p = getParent(current);
            children[p].push_back(current);
            current = p;
        }
    }
}

vector<string> NodeSetCouplingGraph::getChildren(const string& node) {
    auto found = children.find(node);
    if (found == children.end()) {
        createChildCache();
        found = children.find(node);
        if (found == children.end()) {
            return {};
        }
    }
    return found->second;
}
