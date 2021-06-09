#include <iostream>
#include "NormalizeSupportWithChildren.h"

float NormalizeSupportWithChildren::getAbsoluteSupport(const string& node) {
    auto cacheEntry = absoluteSupportPerNodeCache.find(node);
    if (cacheEntry != absoluteSupportPerNodeCache.end()) {
        return cacheEntry->second;
    }
    float result = getAbsoluteSelfSupport(node);
    for (const auto& child: getChildren(node)) {
        result += getAbsoluteSupport(child);
    }
    absoluteSupportPerNodeCache[node] = result;
    std::cout << "getAbsoluteSupport " << node << ": " << result << endl;
    return result;
}