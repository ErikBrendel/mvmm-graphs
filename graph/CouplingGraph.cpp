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

string CouplingGraph::getParent(const string& node) {
    if (node.length() <= 1) {
        return "";
    }
    auto parts = split(node, "/");
    parts.pop_back();
    return join(parts, "/");
}

string picklePathParentDir(const string& repoName, const string& name, const string& storageDir) {
    return storageDir + repoName;
}

string picklePath(const string& repoName, const string& name, const string& storageDir) {
    return storageDir + repoName + "/" + name + ".graph";
}

void CouplingGraph::save(const string& repoName, const string& storageDir) {
    string fullPath = picklePath(repoName, name, storageDir);
    experimental::filesystem::create_directories(picklePathParentDir(repoName, name, storageDir));
    ofstream out(picklePath(repoName, name, storageDir), ios::trunc);
    plaintextSave(out);
    out.close();
}

shared_ptr<CouplingGraph> CouplingGraph::load(const string& repoName, const string& name, const string& storageDir) {
    string fullPath = picklePath(repoName, name, storageDir);
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

double CouplingGraph::howWellPredictsMissingNode(const vector<string>& nodeSet, const string& nodeMissingFromSet,
                                                const vector<string>& allNodes) {

    vector<string> couplingSortedNodes(allNodes);

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
