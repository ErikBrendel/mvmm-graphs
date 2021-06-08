#include "DataStorage.h"

int DataStorage::load(const string& repoName, const string& name) {
    return insertG(CouplingGraph::load(repoName, name));
}

void DataStorage::save(int g, const string& repoName) {
    getG(g)->save(repoName);
}

const shared_ptr<CouplingGraph>& DataStorage::getG(int g) const {
    if (g < 0 || g >= graphArray.size()) {
        throw runtime_error("Graph index " + to_string(g) + " out of range!");
    }
    return graphArray[g];
}

const shared_ptr<CouplingGraph>& DataStorage::getG(const string& gString) const {
    return getG(stoi(gString));
}

int DataStorage::createExplicit(const string& name) {
    return insertG(make_shared<ExplicitCouplingGraph>(name));
}

int DataStorage::createSimilarity(const string& name) {
    return insertG(make_shared<SimilarityCouplingGraph>(name));
}

int DataStorage::createModuleDistance() {
    return insertG(make_shared<ModuleDistanceCouplingGraph>());
}


int DataStorage::createCached(int g) {
    return insertG(make_shared<ResultCachedGraph>(getG(g)));
}

int DataStorage::createCombination(const vector<int>& gs, const vector<float>& weights) {
    vector<shared_ptr<CouplingGraph>> graphs(gs.size());
    rep(i, gs.size()) {
        graphs[i] = getG(gs[i]);
    }
    return insertG(make_shared<WeightCombinedGraph>(graphs, weights));
}

int DataStorage::insertG(const shared_ptr<CouplingGraph>& graph) {
    graphArray.push_back(graph);
    return (int)graphArray.size() - 1;
}

const shared_ptr<vector<string>>& DataStorage::getNodeSet(int n) const {
    if (n < 0 || n >= nodeSetArray.size()) {
        throw runtime_error("NodeSet index " + to_string(n) + " out of range!");
    }
    return nodeSetArray[n];
}

const shared_ptr<vector<string>>& DataStorage::getNodeSet(const string& nString) const {
    return getNodeSet(stoi(nString));
}

int DataStorage::addNodeSet(const shared_ptr<vector<string>>& nodeSet) {
    nodeSetArray.push_back(nodeSet);
    return (int)nodeSetArray.size() - 1;
}
