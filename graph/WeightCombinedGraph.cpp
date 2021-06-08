#include <iostream>
#include "WeightCombinedGraph.h"
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
                                         const vector<float>& weights)
                                         : CouplingGraph("Combined Graph"), graphs{graphs}, weights{weights} {
    if (this->weights.empty()) {
        float defaultWeights = 1.0f / (float)this->graphs.size();
        this->weights.resize(graphs.size());
        fill(all(this->weights), defaultWeights);
    }
}

const vector<string>& WeightCombinedGraph::getNodeSet() {
    if (combinedNodeSet.empty()) {
        combinedNodeSet = getGraphNodeSetCombination(graphs);
    }
    return combinedNodeSet;
}

float WeightCombinedGraph::getNormalizedSupport(const string& node) {
    float result = 0;
    rep(i, graphs.size()) {
        if (weights[i] != 0) {
            result += weights[i] * graphs[i]->getNormalizedSupport(node);
        }
    }
    return result;
}

float WeightCombinedGraph::getNormalizedCoupling(const string& a, const string& b) {
    float result = 0;
    rep(i, graphs.size()) {
        if (weights[i] != 0) {
            result += weights[i] * graphs[i]->getNormalizedCoupling(a, b);
        }
    }
    return result;
}

string WeightCombinedGraph::plaintextContent() {
    throw runtime_error("Cannot serialize combined graph!");
}

void WeightCombinedGraph::printStatistics() {
    cout << "Combined graph statistics WIP" << endl;
}
