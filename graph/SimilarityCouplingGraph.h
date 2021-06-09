#pragma once

#include "CouplingGraph.h"
#include "graphMixins/NormalizeSupport.h"

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class SimilarityCouplingGraph : public NormalizeSupport, public CouplingGraph {
private:
    vector<string> nodes;
    unordered_map<string, vector<float>> coords;
    unordered_map<string, float> support;

public:
    explicit SimilarityCouplingGraph(const string& name);

    const vector<string> & getNodeSet() override;
    void addNode(const string& node, const vector<float>& coords, float support);
    float getSupport(const string& node);
    float getAbsoluteSupport(const string &node) override;
    float getNormalizedCoupling(const string &a, const string &b) override;
    void plaintextContent(ostream& out) override;
    void printStatistics() override;

    float getNormalizedSupport(const string &node) override;
};


