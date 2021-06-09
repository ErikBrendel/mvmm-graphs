#pragma once

#include "CouplingGraph.h"
#include "CouplingGraph.h"

#include <unordered_set>

using namespace std;


vector<string> getGraphNodeSetCombination(const vector<shared_ptr<CouplingGraph>>& graphs);


class WeightCombinedGraph : public CouplingGraph {
private:
    vector<shared_ptr<CouplingGraph>> graphs;
    vector<float> weights;
    vector<string> combinedNodeSet;

public:
    explicit WeightCombinedGraph(const vector<shared_ptr<CouplingGraph>>& graphs, const vector<float>& weights = {});

    void setWeights(const vector<float>& newWeights);

    const vector<string> & getNodeSet() override;
    float getNormalizedSupport(const string &node) override;
    float getNormalizedCoupling(const string &a, const string &b) override;
    void plaintextContent(ostream& out) override;
    void printStatistics() override;
};


