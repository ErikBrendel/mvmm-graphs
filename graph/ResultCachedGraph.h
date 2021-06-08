#pragma once

#include "CouplingGraph.h"
#include "../util.h"

#include <unordered_map>

using namespace std;

class ResultCachedGraph : public CouplingGraph {
private:
    shared_ptr<CouplingGraph> wrapped;
    unordered_map<string, float> supportCache;
    unordered_map<pair<string, string>, float, pair_hash> couplingCache;

public:
    explicit ResultCachedGraph(const shared_ptr<CouplingGraph>& wrapped);

    const vector<string> & getNodeSet() override;
    float getNormalizedSupport(const string &node) override;
    float getNormalizedCoupling(const string &a, const string &b) override;
    string plaintextContent() override;
    void printStatistics() override;
};


