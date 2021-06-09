#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

using namespace std;

class NormalizeCouplingWithChildren {
private:
    unordered_map<string, double> total_relative_coupling_cache;
public:

    virtual const vector<string>& getNodeSet() = 0;
    virtual const unordered_set <string>& getChildren(const string& node) = 0;
    virtual vector<string> getDirectlyCoupled(const string& node) = 0;
    virtual double getDirectCoupling(const string& a, const string& b) = 0;

    virtual double getNormalizedCoupling(const string& a, const string& b);
private:
    vector<string> getCouplingCandidates(const string& node, bool addPredecessors = false);
    vector<string> getSelfAndDescendants(const string& node);
    void getSelfAndDescendentsRec(const string& node, vector<string>& result);
    double getDirectMultiCoupling(const string& a, const vector<string>& others);
    double getRelativeCoupling(const string& a, const string& b);
    double getRelativeMultiCoupling(const string& a, const vector<string>& others);
    double getRelativeMultiDirectCoupling(const string& a, const vector<string>& others);
    double getTotalRelativeCoupling(const string& a);
};
