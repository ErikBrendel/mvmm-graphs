#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class NormalizeCouplingWithChildren {
private:
    unordered_map<string, float> total_relative_coupling_cache;
public:

    virtual vector<string> getChildren(const string& node) = 0;
    virtual vector<string> getDirectlyCoupled(const string& node) = 0;
    virtual float getDirectCoupling(const string& a, const string& b) = 0;

    virtual float getNormalizedCoupling(const string& a, const string& b);
private:
    vector<string> getCouplingCandidates(const string& node, bool addPredecessors = false);
    vector<string> getSelfAndDescendants(const string& node);
    void getSelfAndDescendentsRec(const string& node, vector<string>& result);
    float getDirectMultiCoupling(const string& a, const vector<string>& others);
    float getRelativeCoupling(const string& a, const string& b);
    float getRelativeMultiCoupling(const string& a, const vector<string>& others);
    float getRelativeMultiDirectCoupling(const string& a, const vector<string>& others);
    float getTotalRelativeCoupling(const string& a);
};
