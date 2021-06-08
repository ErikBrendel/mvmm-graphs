#include "GraphStorage.h"

int GraphStorage::load(const string& repoName, const string& name) {
    return insert(CouplingGraph::load(repoName, name));
}

void GraphStorage::save(int g, const string& repoName) {
    get(g)->save(repoName);
}

const shared_ptr<CouplingGraph>& GraphStorage::get(int g) const {
    if (g < 0 || g >= graphArray.size()) {
        throw runtime_error("Graph index " + to_string(g) + " out of range!");
    }
    return graphArray[g];
}

const shared_ptr<CouplingGraph>& GraphStorage::get(const string& gString) const {
    return get(stoi(gString));
}

int GraphStorage::createExplicit(const string& name) {
    return insert(make_shared<ExplicitCouplingGraph>(name));
}

int GraphStorage::createSimilarity(const string& name) {
    return insert(make_shared<SimilarityCouplingGraph>(name));
}

int GraphStorage::createModuleDistance() {
    return insert(make_shared<ModuleDistanceCouplingGraph>());
}


int GraphStorage::createCached(int g) {
    return insert(make_shared<ResultCachedGraph>(get(g)));
}

int GraphStorage::createCombination(const vector<int>& gs, const vector<float>& weights) {
    vector<shared_ptr<CouplingGraph>> graphs(gs.size());
    rep(i, gs.size()) {
        graphs[i] = get(gs[i]);
    }
    return insert(make_shared<WeightCombinedGraph>(graphs, weights));
}

int GraphStorage::insert(const shared_ptr<CouplingGraph>& graph) {
    graphArray.push_back(graph);
    return (int)graphArray.size() - 1;
}
