#include "ResultCachedGraph.h"

ResultCachedGraph::ResultCachedGraph(const shared_ptr<CouplingGraph>& wrapped)
    : CouplingGraph(wrapped->getName()), wrapped{wrapped} {}

const vector<string>& ResultCachedGraph::getNodeSet() {
    return wrapped->getNodeSet();
}

float ResultCachedGraph::getNormalizedSupport(const string& node) {
    auto found = supportCache.find(node);
    if (found == supportCache.end()) {
        float support = wrapped->getNormalizedSupport(node);
        found->second = support;
        return support;
    } else {
        return found->second;
    }
}

float ResultCachedGraph::getNormalizedCoupling(const string& a, const string& b) {
    pair<string, string> key = make_pair(a, b);
    auto found = couplingCache.find(key);
    if (found == couplingCache.end()) {
        float coupling = wrapped->getNormalizedCoupling(a, b);
        found->second = coupling;
        return coupling;
    } else {
        return found->second;
    }
}

void ResultCachedGraph::plaintextContent(ostream& out) {
    throw runtime_error("Cannot save graph wrapper!");
}

void ResultCachedGraph::printStatistics() {
    // nothing
}
