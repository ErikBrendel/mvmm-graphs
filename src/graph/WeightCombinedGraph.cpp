#include "WeightCombinedGraph.h"

#include <iostream>

#include "../util.h"

vector<string> getGraphNodeSetCombination(const vector<shared_ptr<CouplingGraph>>& graphs) {
    unordered_set<string> uniqueData;
    for (const auto& g: graphs) {
        for (const auto& node: g->getNodeSet()) {
            uniqueData.insert(node);
        }
    }
    vector<string> result(all(uniqueData));
    return result;
}

WeightCombinedGraph::WeightCombinedGraph(const vector<shared_ptr<CouplingGraph>>& graphs,
                                         const vector<double>& weights)
                                         : CouplingGraph("Combined Graph"), graphs{graphs}, weights{weights} {
    if (this->weights.empty()) {
        double defaultWeights = 1.0 / (double)this->graphs.size();
        this->weights.resize(graphs.size());
        fill(all(this->weights), defaultWeights);
    }
}

void WeightCombinedGraph::setWeights(const vector<double>& newWeights) {
    weights = newWeights;
}

const vector<string>& WeightCombinedGraph::getNodeSet() {
    if (combinedNodeSet.empty()) {
        combinedNodeSet = getGraphNodeSetCombination(graphs);
    }
    return combinedNodeSet;
}

vector<string> WeightCombinedGraph::getCouplingCandidates(const string& node, bool addPredecessors) {
    unordered_set<string> allCandidates;
    for (const auto& g: graphs) {
        vector<string> candidates = g->getCouplingCandidates(node, addPredecessors);
        if (candidates.empty()) {
            return {};
        }
        for (const auto& candidate: candidates) {
            allCandidates.insert(candidate);
        }
    }
    return vector<string>(all(allCandidates));
}

double WeightCombinedGraph::getNormalizedSupport(const string& node) {
    double result = 0;
    rep(i, graphs.size()) {
        if (weights[i] != 0) {
            result += weights[i] * graphs[i]->getNormalizedSupport(node);
        }
    }
    return result;
}

double WeightCombinedGraph::getNormalizedCoupling(const string& a, const string& b) {
    double result = 0;
    rep(i, graphs.size()) {
        if (weights[i] != 0) {
            result += weights[i] * graphs[i]->getNormalizedCoupling(a, b);
        }
    }
    return result;
}

void WeightCombinedGraph::printStatistics() {
    cout << "Combined Graph:" << endl;
    cout << "  " << join(graphs, ", ", [](const shared_ptr<CouplingGraph>& g) { return g->getName(); }) << endl;
    cout << "  " << join(weights, ", ") << endl;
}
