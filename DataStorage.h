#pragma once

#include "util.h"

#include "graph/CouplingGraph.h"
#include "graph/ExplicitCouplingGraph.h"
#include "graph/SimilarityCouplingGraph.h"
#include "graph/ModuleDistanceCouplingGraph.h"
#include "graph/WeightCombinedGraph.h"
#include "graph/ResultCachedGraph.h"

#include <vector>
#include <memory>

using namespace std;

class DataStorage {
private:
    vector<shared_ptr<CouplingGraph>> graphArray;
    vector<shared_ptr<vector<string>>> nodeSetArray;

public:
    int load(const string& repoName, const string& name);
    void save(int g, const string& repoName);
    [[nodiscard]] const shared_ptr<CouplingGraph>& getG(int g) const;
    [[nodiscard]] const shared_ptr<CouplingGraph>& getG(const string& gString) const;

    int createExplicit(const string& name);
    int createSimilarity(const string& name);
    int createModuleDistance();
    int createCached(int g);
    int createCombination(const vector<int>& gs, const vector<float>& weights = {});

    [[nodiscard]] const shared_ptr<vector<string>>& getNodeSet(int n) const;
    [[nodiscard]] const shared_ptr<vector<string>>& getNodeSet(const string& nString) const;
    int addNodeSet(const shared_ptr<vector<string>>& nodeSet);

private:
    int insertG(const shared_ptr<CouplingGraph>& graph);
};


