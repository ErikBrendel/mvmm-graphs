#include "CouplingGraph.h"

#include "util.h"

vector<string> emptyNodeSet;

CouplingGraph::CouplingGraph(string name) : name{std::move(name)} {

}

const vector<string>& CouplingGraph::getNodeSet() {
    return emptyNodeSet;
}

string CouplingGraph::getParent(const string& node) {
    if (node.length() <= 1) {
        return "";
    }
    auto parts = split(node, "/");
    parts.pop_back();
    return join(parts, "/");
}
