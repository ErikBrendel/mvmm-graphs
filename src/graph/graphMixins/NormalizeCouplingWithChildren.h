#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

using namespace std;

class NormalizeCouplingWithChildren {
private:
    unordered_map<string, double> simpleTotalOutwardsCouplingCache;
public:

    virtual const vector<string>& getNodeSet() = 0;
    virtual vector<string> getSelfAndDescendants(const string& node) = 0;
    virtual vector<pair<string, double>> getEdges(const string& node) = 0;
    virtual double getDirectCoupling(const string& a, const string& b) = 0;

    virtual double getNormalizedCoupling(const string& a, const string& b);
private:
    double getTotalOutwardsCoupling(const string& name, const vector<string>& set, bool simpleCase);
    double calculateTotalOutwardsCoupling(const vector<string>& set);
};
