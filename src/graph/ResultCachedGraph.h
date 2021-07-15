#pragma once

#include <unordered_map>

#include "CouplingGraph.h"
#include "../util.h"

using namespace std;

class ResultCachedGraph : public CouplingGraph {
private:
    shared_ptr<CouplingGraph> wrapped;
    unordered_map<string, double> supportCache;
    unordered_map<string, unordered_map<string, double>> couplingCache;

public:
    explicit ResultCachedGraph(const shared_ptr<CouplingGraph>& wrapped);

    const vector<string>& getNodeSet() override;
    vector<string> getCouplingCandidates(const string& node, bool addPredecessors) override;
    double getNormalizedSupport(const string &node) override;
    double getNormalizedCoupling(const string &a, const string &b) override;
    void printStatistics() override;
};


