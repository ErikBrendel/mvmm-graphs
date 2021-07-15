#include "ResultCachedGraph.h"

ResultCachedGraph::ResultCachedGraph(const shared_ptr<CouplingGraph>& wrapped)
    : CouplingGraph(wrapped->getName()), wrapped{wrapped} {}

const vector<string>& ResultCachedGraph::getNodeSet() {
    return wrapped->getNodeSet();
}

vector<string> ResultCachedGraph::getCouplingCandidates(const string& node, bool addPredecessors) {
    return wrapped->getCouplingCandidates(node, addPredecessors);
}

double ResultCachedGraph::getNormalizedSupport(const string& node) {
    // TODO add hints to "if" what is more likely for the cpu pipeline
    auto found = supportCache.find(node);
    if (found == supportCache.end()) {
        double support = wrapped->getNormalizedSupport(node);
        supportCache[node] = support;
        return support;
    } else {
        return found->second;
    }
}

double ResultCachedGraph::getNormalizedCoupling(const string& a, const string& b) {
    // TODO add hints to "if" what is more likely for the cpu pipeline
    auto foundA = couplingCache.find(a);
    if (foundA == couplingCache.end()) {
        double coupling = wrapped->getNormalizedCoupling(a, b);
        couplingCache[a][b] = coupling;
        return coupling;
    } else {
        auto& aMap = foundA->second;
        auto foundB = aMap.find(b);
        if (foundB == aMap.end()) {
            double coupling = wrapped->getNormalizedCoupling(a, b);
            aMap[b] = coupling;
            return coupling;
        } else {
            return foundB->second;
        }
    }
}

void ResultCachedGraph::printStatistics() {
    cout << "Result cached graph of: " << wrapped->getName() << endl;
}
