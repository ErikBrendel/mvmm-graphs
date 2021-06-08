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

class GraphStorage {
private:
    vector<shared_ptr<CouplingGraph>> graphArray;

public:
    int load(const string& repoName, const string& name);
    void save(int g, const string& repoName);
    [[nodiscard]] const shared_ptr<CouplingGraph>& get(int g) const;

    int createExplicit(const string& name);
    int createSimilarity(const string& name);
    int createModuleDistance();
    int createCached(int g);
    int createCombination(const vector<int>& gs, const vector<float>& weights = {});

private:
    int insert(const shared_ptr<CouplingGraph>& graph);
};


