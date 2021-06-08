#pragma once

#include <vector>
#include <string>

#include "NodeSetCouplingGraph.h"
#include "graphMixins/NormalizeSupportWithChildren.h"
#include "graphMixins/NormalizeCouplingWithChildren.h"

using namespace std;

class ExplicitCouplingGraph : public NormalizeCouplingWithChildren, public NormalizeSupportWithChildren, public NodeSetCouplingGraph {
private:
    vector<float> supports;
    vector<vector<pair<uint, float>>> adj;
    unordered_map<string, uint> node2i;
    vector<string> i2node;
public:
    explicit ExplicitCouplingGraph(const string& name);

    const vector<string>& getNodeSet() override;
    void add(const string& a, const string& b, float delta);
    float get(const string& a, const string& b);
    float getDirectCoupling(const string& a, const string& b) override;
    vector<string> getDirectlyCoupled(const string& node);

    void addSupport(const string& node, float delta);
    float getSupport(const string& node);
    float getAbsoluteSelfSupport(const string& node) override;

    void addAndSupport(const string& a, const string& b, float delta);

    void cutoffEdges(float minimumWeight);
    void propagateDown(int layers = 1, float weightFactor = 0.2f);
    void dilate(int iterations = 1, float weightFactor = 0.2f);


    string plaintextContent() override;
    void printStatistics() override;

    vector<string> getChildren(const string &node) override;
    float getNormalizedSupport(const string& node) override;
    float getNormalizedCoupling(const string& a, const string& b) override;

private:
    uint getNodeIndex(const string& node);
    void addI(uint a, uint b, float delta);
    unordered_map<uint, vector<uint>> getChildrenDict();
};


