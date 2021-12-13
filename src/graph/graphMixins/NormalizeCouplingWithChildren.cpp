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

double NormalizeCouplingWithChildren::getRelativeCoupling(const string& a, const string& b) {
    auto [aDesc, bDesc] = getDisjointDescendents(a, b);
    double result = 0;
    for (const auto& ad: aDesc) {
        for (const auto& bd: bDesc) {
            result += getDirectCoupling(ad, bd);
        }
    }
    return result;
}

double NormalizeCouplingWithChildren::getTotalOutwardsCoupling(const vector<string>& set) {
    double result = 0;
    for (const auto& elem: set) {
        for (const auto& [other, weight]: getEdges(elem)) {
            if (find(all(set), other) == set.end()) {
                result += weight;
            }
        }
    }
    return result;
}

double NormalizeCouplingWithChildren::getNormalizedCoupling(const string& a, const string& b) {
    const auto& nodeSet = getNodeSet();
    if (find(all(nodeSet), a) == nodeSet.end() || find(all(nodeSet), b) == nodeSet.end()) {
        return 0;
    }
    double targetCoupling = getRelativeCoupling(a, b);
    double targetCoupling2 = getRelativeCoupling(b, a);
    ensure(targetCoupling == targetCoupling2,
           "Relative coupling seems not symmetric! " + to_string(targetCoupling) + " != " + to_string(targetCoupling2) + " (a=" + a + ", b=" + b + ")")
    if (targetCoupling == 0) return 0;

    auto [aDesc, bDesc] = getDisjointDescendents(a, b);


    double totalCouplingA = getTotalOutwardsCoupling(aDesc);
    double totalCouplingB = getTotalOutwardsCoupling(bDesc);
    ensure(targetCoupling <= totalCouplingA && targetCoupling <= totalCouplingB, "Coupling value is bigger than the total coupling!")
    // return the biggest of both possible normalized coupling values
    return targetCoupling / min(totalCouplingA, totalCouplingB);
}

pair<vector<string>, vector<string>> NormalizeCouplingWithChildren::getDisjointDescendents(const string& a, const string& b) {
    auto aDesc = getSelfAndDescendants(a);
    sort(all(aDesc));
    auto bDesc = getSelfAndDescendants(b);
    sort(all(bDesc));
    if (startsWith(a, b)) {
        vectorDifference(bDesc, aDesc);
    } else if (startsWith(b, a)) {
        vectorDifference(aDesc, bDesc);
    }
    return {aDesc, bDesc};
}
