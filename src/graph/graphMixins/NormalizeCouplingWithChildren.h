#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

using namespace std;

class NormalizeCouplingWithChildren {
public:

    virtual const vector<string>& getNodeSet() = 0;
    virtual vector<string> getCouplingCandidates(const string& node, bool addPredecessors);
    virtual const unordered_set <string>& getChildren(const string& node) = 0;
    virtual vector<string> getDirectlyCoupled(const string& node) = 0;
    virtual vector<pair<string, double>> getEdges(const string& node) = 0;
    virtual double getDirectCoupling(const string& a, const string& b) = 0;

    virtual double getNormalizedCoupling(const string& a, const string& b);
private:
    vector<string> getSelfAndDescendants(const string& node);
    void getSelfAndDescendentsRec(const string& node, vector<string>& result);
    double getRelativeCoupling(const string& a, const string& b);
    double getTotalOutwardsCoupling(const vector<string>& set);
    pair<vector<string>, vector<string>> getDisjointDescendents(const string& a, const string& b);
};
