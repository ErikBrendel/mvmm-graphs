#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "CouplingGraph.h"
#include "graphMixins/NormalizeSupport.h"

using namespace std;

class SimilarityCouplingGraph : public NormalizeSupport, public CouplingGraph {
private:
    vector<string> nodes;
    unordered_map<string, vector<double>> coords;
    unordered_map<string, double> support;

public:
    explicit SimilarityCouplingGraph(const string& name);

    const vector<string> & getNodeSet() override;
    void addNode(const string& node, const vector<double>& coords, double support);
    double getSupport(const string& node);
    double getAbsoluteSupport(const string &node) override;
    double getNormalizedCoupling(const string &a, const string &b) override;

    void plaintextSave(ostream& out) override;
    void plaintextLoad(istream& in) override;
    void printStatistics() override;

    double getNormalizedSupport(const string &node) override;
};


