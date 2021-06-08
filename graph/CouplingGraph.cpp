#include "CouplingGraph.h"

#include <utility>

#include "../util.h"

vector<string> emptyNodeSet;

CouplingGraph::CouplingGraph(string name) : name{std::move(name)} {}

const string& CouplingGraph::getName() const {
    return name;
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

void CouplingGraph::save(const string& repoName) {
    throw runtime_error("Saving is not yet implemented!");
}

shared_ptr<CouplingGraph> CouplingGraph::load(const string& repoName, const string& name) {
    throw runtime_error("Loading is not yet implemented!");
}

string CouplingGraph::picklePath(const string& repoName, const string& name) {
    return repoName + "/" + name; // TODO!
}

void CouplingGraph::plaintextSave(const string& repoName) {
    throw runtime_error("Cannot plaintext save yet!");
}

float CouplingGraph::howWellPredictsMissingNode(const vector<string>& nodeSet, const string& nodeMissingFromSet,
                                                const vector<string>& allNodes) {

    vector<string> couplingSortedNodes(allNodes);

    sort(all(couplingSortedNodes), [&](const string& a, const string& b) {
        float couplingSumA = 0;
        float couplingSumB = 0;
        for (const auto& setNode: nodeSet) {
            couplingSumA += getNormalizedCoupling(setNode, a);
            couplingSumB += getNormalizedCoupling(setNode, b);
        }
        if (couplingSumA == couplingSumB) {
            if (a == nodeMissingFromSet) return false;
            if (b == nodeMissingFromSet) return true;
        }
        return couplingSumA > couplingSumB;
    });
    auto resultIndex = (float)distance(couplingSortedNodes.begin(), find(all(couplingSortedNodes), nodeMissingFromSet));
    return 1 - (resultIndex / (float)couplingSortedNodes.size());
}
