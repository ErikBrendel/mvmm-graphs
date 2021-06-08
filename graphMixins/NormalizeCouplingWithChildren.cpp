#include "NormalizeCouplingWithChildren.h"

#include "../util.h"
#include "../CouplingGraph.h"
#include <unordered_set>

using namespace std;

vector<string> NormalizeCouplingWithChildren::getCouplingCandidates(const string& node, bool addPredecessors) {
    vector<string> thisAndDescendants = getSelfAndDescendants(node);

    vector<string> directCouplingCandidates;
    for (const auto& n: thisAndDescendants) {
        for (const auto& n2: getDirectlyCoupled(n)) {
            directCouplingCandidates.push_back(n2);
        }
    }

    unordered_set<string> result;
    result.insert("");
    for (const auto& other: directCouplingCandidates) {
        string current = other;
        while (result.insert(other).second) {  // while not present yet
            if (!addPredecessors) break;
            current = CouplingGraph::getParent(current);
        }
    }
    result.erase("");
    vector<string> resultVec(all(result));

    return resultVec;
}

vector<string> NormalizeCouplingWithChildren::getSelfAndDescendants(const string& node) {
    vector<string> result;
    getSelfAndDescendentsRec(node, result);
    return result;
}

void NormalizeCouplingWithChildren::getSelfAndDescendentsRec(const string& node, vector<string>& result) {
    result.push_back(node);
    for (const auto& child: getChildren(node)) {
        getSelfAndDescendentsRec(child, result);
    }
}

float NormalizeCouplingWithChildren::getDirectMultiCoupling(const string& a, const vector<string>& others) {
    float result = 0;
    for (const auto& b : others) {
        result += getDirectCoupling(a, b);
    }
    return result;
}

float NormalizeCouplingWithChildren::getRelativeCoupling(const string& a, const string& b) {
    return getRelativeMultiDirectCoupling(a, getSelfAndDescendants(b));
}

float NormalizeCouplingWithChildren::getRelativeMultiCoupling(const string& a, const vector<string>& others) {
    vector<string> directOthers;
    for (const auto& other: others) {
        getSelfAndDescendentsRec(other, directOthers);
    }
    return getRelativeMultiDirectCoupling(a, directOthers);
}

float NormalizeCouplingWithChildren::getRelativeMultiDirectCoupling(const string& a, const vector<string>& others) {
    if (others.empty()) {
        return 0;
    }
    float result = getDirectMultiCoupling(a, others);
    for (const auto& child: getChildren(a)) {
        result += getRelativeMultiDirectCoupling(child, others);
    }
    return result;
}

float NormalizeCouplingWithChildren::getTotalRelativeCoupling(const string& a) {
    auto cacheElement = total_relative_coupling_cache.find(a);
    if (cacheElement != total_relative_coupling_cache.end()) {
        return cacheElement->second;
    }

    vector<string> aCandidates = getCouplingCandidates(a, false);
    float total_coupling = getRelativeMultiDirectCoupling(a, aCandidates);
    cacheElement->second = total_coupling;
    return total_coupling;
}

float NormalizeCouplingWithChildren::getNormalizedCoupling(const string& a, const string& b) {
    // TODO         if a not in self.g or b not in self.g: return 0
    float targetCoupling = getRelativeCoupling(a, b);
    if (targetCoupling == 0) return 0;
    float totalCoupling = getTotalRelativeCoupling(a);
    return targetCoupling / totalCoupling;
}
