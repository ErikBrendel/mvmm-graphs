#include "ResultCachedGraph.h"

ResultCachedGraph::ResultCachedGraph(const shared_ptr<CouplingGraph>& wrapped)
    : CouplingGraph(wrapped->getName()), wrapped{wrapped} {}

const vector<string>& ResultCachedGraph::getNodeSet() {
    return wrapped->getNodeSet();
}

double ResultCachedGraph::getNormalizedSupport(const string& node) {
    auto found = supportCache.find(node);
    if (found == supportCache.end()) {
        double support = wrapped->getNormalizedSupport(node);
        found->second = support;
        return support;
    } else {
        return found->second;
    }
}

double ResultCachedGraph::getNormalizedCoupling(const string& a, const string& b) {
    pair<string, string> key = make_pair(a, b);
    auto found = couplingCache.find(key);
    if (found == couplingCache.end()) {
        double coupling = wrapped->getNormalizedCoupling(a, b);
        found->second = coupling;
        return coupling;
    } else {
        return found->second;
    }
}

void ResultCachedGraph::printStatistics() {
    // nothing
}
