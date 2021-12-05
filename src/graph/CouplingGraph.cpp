#include "CouplingGraph.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include "ExplicitCouplingGraph.h"
#include "SimilarityCouplingGraph.h"
#include "../util.h"

vector<string> emptyNodeSet;

CouplingGraph::CouplingGraph(string name) : name{std::move(name)} {}

const string& CouplingGraph::getName() const {
    return name;
}

const vector<string>& CouplingGraph::getNodeSet() {
    return emptyNodeSet;
}

vector<string> CouplingGraph::getCouplingCandidates(const string& node, bool addPredecessors) {
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

string saveLocationParentDir(const string& repoName, const string& storageDir) {
    return storageDir + repoName;
}

string CouplingGraph::saveLocation(const string& repoName, const string& name, const string& storageDir) {
    return saveLocationParentDir(repoName, storageDir) + "/" + name + ".graph";
}

void CouplingGraph::save(const string& repoName, const string& storageDir) {
    string fullPath = saveLocation(repoName, name, storageDir);
    experimental::filesystem::create_directories(saveLocationParentDir(repoName, storageDir));
    ofstream out(saveLocation(repoName, name, storageDir), ios::trunc);
    out.precision(10);
    out << fixed;
    plaintextSave(out);
    out.flush();
    out.close();
}

shared_ptr<CouplingGraph> CouplingGraph::load(const string& repoName, const string& name, const string& storageDir) {
    string fullPath = saveLocation(repoName, name, storageDir);
    ifstream input(fullPath);
    string type;
    getline(input, type);
    shared_ptr<CouplingGraph> result;
    if (type == "Explicit") {
        result = make_shared<ExplicitCouplingGraph>(name);
    } else if (type == "Similarity") {
        result = make_shared<SimilarityCouplingGraph>(name);
    } else {
        throw runtime_error("Unknown graph type! " + type);
    }
    result->plaintextLoad(input);
    input.close();
    return result;
}

void CouplingGraph::plaintextSave(ostream& out) {
    throw runtime_error("Cannot save this kind of graph!");
}

void CouplingGraph::plaintextLoad(istream& in) {
    throw runtime_error("Cannot load this kind of graph!");
}

vector<tuple<double, string, string>> CouplingGraph::getMostLinkedNodePairs(int amount) {
    auto nodeSet = getNodeSet();
    if (nodeSet.size() < 2) {
        return {};
    }
    vector<tuple<double, string, string>> sortedData;
    rep_all_pairs(i, j, nodeSet.size()) {
        const auto& a = nodeSet[i];
        const auto& b = nodeSet[j];
        sortedData.emplace_back(getNormalizedCoupling(a, b), a, b);
        if (sortedData.size() > amount) {
            sort(all(sortedData), greater<>());
            sortedData.pop_back();
        }
    }
    while (sortedData.size() > amount) {
        sortedData.pop_back();
    }
    return sortedData;
}

double CouplingGraph::howWellPredictsMissingNode(const vector<string>& nodeSet, const string& nodeMissingFromSet,
                                                const vector<string>& allNodes) {

    vector<string> couplingSortedNodes(allNodes);

    // TODO calculate k rank hit could be done in linear time - better?
    sort(all(couplingSortedNodes), [&](const string& a, const string& b) {
        double couplingSumA = 0;
        double couplingSumB = 0;
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
    auto resultIndex = (double)distance(couplingSortedNodes.begin(), find(all(couplingSortedNodes), nodeMissingFromSet));
    return 1 - (resultIndex / (double)couplingSortedNodes.size());
}
