#pragma once

#include "CouplingGraph.h"
#include "CouplingGraph.h"

#include <unordered_set>

using namespace std;


vector<string> getGraphNodeSetCombination(const vector<shared_ptr<CouplingGraph>>& graphs);


class WeightCombinedGraph : public CouplingGraph {
private:
    vector<shared_ptr<CouplingGraph>> graphs;
    vector<double> weights;
    vector<string> combinedNodeSet;

public:
    explicit WeightCombinedGraph(const vector<shared_ptr<CouplingGraph>>& graphs, const vector<double>& weights = {});

    void setWeights(const vector<double>& newWeights);

    const vector<string> & getNodeSet() override;
    double getNormalizedSupport(const string &node) override;
    double getNormalizedCoupling(const string &a, const string &b) override;
    void printStatistics() override;
};


