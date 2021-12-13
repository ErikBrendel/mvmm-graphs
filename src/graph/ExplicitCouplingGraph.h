#pragma once

#include <vector>
#include <string>

#include "NodeSetCouplingGraph.h"
#include "graphMixins/NormalizeSupportWithChildren.h"
#include "graphMixins/NormalizeCouplingWithChildren.h"

using namespace std;

class ExplicitCouplingGraph : public NormalizeCouplingWithChildren, public NormalizeSupportWithChildren, public NodeSetCouplingGraph {
private:
    vector<double> supports;
    /**
     * sparse (map instead of inner vector) symmetric (adj[a][b] == adj[b][a]) adjacency matrix
     */
    vector<unordered_map<uint, double>> adj;
    unordered_map<string, uint> node2i;
    vector<string> i2node;
public:
    explicit ExplicitCouplingGraph(const string& name);

    const vector<string>& getNodeSet() override;
    vector<string> getCouplingCandidates(const string& node, bool addPredecessors) override;
    void add(const string& a, const string& b, double delta);
    double getI(uint a, uint b);
    double getDirectCoupling(const string& a, const string& b) override;
    vector<string> getDirectlyCoupled(const string& node) override;
    vector<pair<string, double>> getEdges(const string& node) override;

    void addSupport(const string& node, double delta);
    double getSupport(const string& node);
    double getAbsoluteSelfSupport(const string& node) override;

    void addAndSupport(const string& a, const string& b, double delta);

    void removeNode(const string& node);

    void cutoffEdges(double minimumWeight);
    void removeSmallComponents(int minimumComponentSize);
    void propagateDown(int layers = 1, double weightFactor = 0.2);
    void dilate(int iterations = 1, double weightFactor = 0.2);


    void plaintextSave(ostream& out) override;
    void plaintextLoad(istream& in) override;
    void printStatistics() override;
    vector<tuple<double, string, string>> getMostLinkedNodePairs(int amount) override;
    vector<unordered_set<uint>> getConnectedComponents();

    const unordered_set<string>& getChildren(const string &node) override;
    vector<string> getSelfAndDescendants(const string &node) override;
    double getNormalizedSupport(const string& node) override;
    double getNormalizedCoupling(const string& a, const string& b) override;

private:
    uint getNodeIndex(const string& node);
    void addI(uint a, uint b, double delta);
    unordered_map<uint, vector<uint>> getChildrenDict();
    unordered_set<uint> plainBfs(uint source);
};


