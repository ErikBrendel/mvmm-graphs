#pragma once

#include <vector>
#include <memory>

#include "graph/CouplingGraph.h"
#include "graph/ExplicitCouplingGraph.h"
#include "graph/SimilarityCouplingGraph.h"
#include "graph/ModuleDistanceCouplingGraph.h"
#include "graph/WeightCombinedGraph.h"
#include "graph/ResultCachedGraph.h"
#include "BestResultSet.h"
#include "disagreementAnalysis.h"

using namespace std;

class DataStorage {
private:
    vector<shared_ptr<CouplingGraph>> graphArray;
    vector<shared_ptr<vector<string>>> nodeSetArray;
    vector<shared_ptr<BestResultSet<BrsUserData>>> bestResultSets;

public:
    int load(const string& repoName, const string& name, const string& storageDir);
    void save(int g, const string& repoName, const string& storageDir);
    [[nodiscard]] const shared_ptr<CouplingGraph>& getG(int g) const;
    [[nodiscard]] const shared_ptr<CouplingGraph>& getG(const string& gString) const;

    int createExplicit(const string& name);
    int createSimilarity(const string& name);
    int createModuleDistance();
    int createCached(int g);
    int createCombination(const vector<int>& gs, const vector<double>& weights = {});

    [[nodiscard]] const shared_ptr<vector<string>>& getNodeSet(int n) const;
    [[nodiscard]] const shared_ptr<vector<string>>& getNodeSet(const string& nString) const;
    int addNodeSet(const shared_ptr<vector<string>>& nodeSet);

    int createBestResultSet(int dimensions, int resultKeepSize);
    [[nodiscard]] shared_ptr<BestResultSet<BrsUserData>> getBestResultSet(int brsIndex) const;

private:
    int insertG(const shared_ptr<CouplingGraph>& graph);
};


