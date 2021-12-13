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

double NormalizeCouplingWithChildren::getTotalOutwardsCoupling(const string& name, const vector<string>& set, bool simpleCase) {
    if (simpleCase) {
        if (simpleTotalOutwardsCouplingCache.find(name) == simpleTotalOutwardsCouplingCache.end()) {
            simpleTotalOutwardsCouplingCache[name] = calculateTotalOutwardsCoupling(set);
        }
        return simpleTotalOutwardsCouplingCache[name];
    } else {
        return calculateTotalOutwardsCoupling(set);
    }
}

double NormalizeCouplingWithChildren::calculateTotalOutwardsCoupling(const vector<string>& set) {
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
    if (a == b) return 0;
    const auto& nodeSet = getNodeSet();
    if (find(all(nodeSet), a) == nodeSet.end() || find(all(nodeSet), b) == nodeSet.end()) {
        return 0;
    }

    bool aStartsWithB = startsWith(a, b);
    bool bStartsWithA = startsWith(b, a);

    auto aDesc = getSelfAndDescendants(a);
    auto bDesc = getSelfAndDescendants(b);
    sort(all(aDesc));
    sort(all(bDesc));
    if (aStartsWithB) {
        vectorDifference(bDesc, aDesc);
    } else if (bStartsWithA) {
        vectorDifference(aDesc, bDesc);
    }
    double relativeCoupling = 0;
    for (const auto& ad: aDesc) {
        for (const auto& bd: bDesc) {
            relativeCoupling += getDirectCoupling(ad, bd);
        }
    }

    if (relativeCoupling == 0) return 0;

    double totalCouplingA = getTotalOutwardsCoupling(a, aDesc, !bStartsWithA);
    double totalCouplingB = getTotalOutwardsCoupling(b, bDesc, !aStartsWithB);

    ensure(relativeCoupling <= totalCouplingA && relativeCoupling <= totalCouplingB, "Coupling value is bigger than the total coupling!")
    // return the biggest of both possible normalized coupling values
    return relativeCoupling / min(totalCouplingA, totalCouplingB);
}
