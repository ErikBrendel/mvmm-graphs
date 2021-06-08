#include "NormalizeSupport.h"
#include "../../util.h"

#include <algorithm>

using namespace std;

float NormalizeSupport::getNormalizedSupport(const string& node) {
    auto cacheEntry = normalizedSupportPerNodeCache.find(node);
    if (cacheEntry != normalizedSupportPerNodeCache.end()) {
        return cacheEntry->second;
    }
    float absSupp = getAbsoluteSupport(node);
    auto [median, maximum] = getAbsoluteSupportMedianAndMax();
    float result;
    if (absSupp <= median) {
        result = 0.5f * absSupp / median;
    } else {
        result = 0.5f + (0.5f * (absSupp - median) / (maximum - median));
    }
    cacheEntry->second = result;
    return result;
}

pair<float, float> NormalizeSupport::getAbsoluteSupportMedianAndMax() {
    if (medianMaximumCache.second == 0) {
        vector<string> nodes = getNodeSet();
        vector<float> supports(nodes.size());
        rep(i, nodes.size()) {
            supports[i] = getAbsoluteSupport(nodes[i]);
        }
        medianMaximumCache.first = accumulate(all(supports), 0.0f) / (float) nodes.size();
        medianMaximumCache.second = *max_element(all(supports));
    }
    return medianMaximumCache;
}
