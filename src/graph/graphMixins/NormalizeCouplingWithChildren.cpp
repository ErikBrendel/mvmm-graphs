#include "NormalizeCouplingWithChildren.h"

#include <unordered_set>

#include "../../util.h"
#include "../CouplingGraph.h"

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

double NormalizeCouplingWithChildren::getDirectMultiCoupling(const string& a, const vector<string>& others) {
    double result = 0;
    for (const auto& b : others) {
        result += getDirectCoupling(a, b);
    }
    return result;
}

double NormalizeCouplingWithChildren::getRelativeCoupling(const string& a, const string& b) {
    return getRelativeMultiDirectCoupling(a, getSelfAndDescendants(b));
}

double NormalizeCouplingWithChildren::getRelativeMultiDirectCoupling(const string& a, const vector<string>& others) {
    if (others.empty()) {
        return 0;
    }
    double result = getDirectMultiCoupling(a, others);
    for (const auto& child: getChildren(a)) {
        result += getRelativeMultiDirectCoupling(child, others);
    }
    return result;
}

double NormalizeCouplingWithChildren::getTotalRelativeCoupling(const string& a) {
    auto cacheElement = totalRelativeCouplingCache.find(a);
    if (cacheElement != totalRelativeCouplingCache.end()) {
        return cacheElement->second;
    }

    vector<string> aCandidates = getCouplingCandidates(a, false);
    double total_coupling = getRelativeMultiDirectCoupling(a, aCandidates);
    totalRelativeCouplingCache[a] = total_coupling;
    return total_coupling;
}

double NormalizeCouplingWithChildren::getNormalizedCoupling(const string& a, const string& b) {
    const auto& nodeSet = getNodeSet();
    if (find(all(nodeSet), a) == nodeSet.end() || find(all(nodeSet), b) == nodeSet.end()) {
        return 0;
    }
    double targetCoupling = getRelativeCoupling(a, b);
    double targetCoupling2 = getRelativeCoupling(a, b);
    ensure(targetCoupling == targetCoupling2,
           "Relative coupling seems not symmetric! " + to_string(targetCoupling) + " != " + to_string(targetCoupling2) + " (a=" + a + ", b=" + b + ")")
    if (targetCoupling == 0) return 0;
    double totalCouplingA = getTotalRelativeCoupling(a);
    double totalCouplingB = getTotalRelativeCoupling(b);
    ensure(targetCoupling > totalCouplingA && targetCoupling > totalCouplingB, "Coupling value is bigger than the total coupling!")
    // return the biggest of both possible normalized coupling values
    return targetCoupling / min(totalCouplingA, totalCouplingB);
}
