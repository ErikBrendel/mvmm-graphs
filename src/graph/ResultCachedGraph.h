#pragma once

#include <unordered_map>

#include "CouplingGraph.h"
#include "../util.h"

using namespace std;

class ResultCachedGraph : public CouplingGraph {
private:
    shared_ptr<CouplingGraph> wrapped;
    unordered_map<string, double> supportCache;
    unordered_map<pair<string, string>, double, pair_hash> couplingCache;

public:
    explicit ResultCachedGraph(const shared_ptr<CouplingGraph>& wrapped);

    const vector<string> & getNodeSet() override;
    double getNormalizedSupport(const string &node) override;
    double getNormalizedCoupling(const string &a, const string &b) override;
    void printStatistics() override;
};


