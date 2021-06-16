#include "NormalizeSupportWithChildren.h"

#include <iostream>

double NormalizeSupportWithChildren::getAbsoluteSupport(const string& node) {
    auto cacheEntry = absoluteSupportPerNodeCache.find(node);
    if (cacheEntry != absoluteSupportPerNodeCache.end()) {
        return cacheEntry->second;
    }
    double result = getAbsoluteSelfSupport(node);
    for (const auto& child: getChildren(node)) {
        result += getAbsoluteSupport(child);
    }
    absoluteSupportPerNodeCache[node] = result;
    return result;
}
