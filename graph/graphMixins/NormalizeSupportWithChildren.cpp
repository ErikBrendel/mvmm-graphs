#include "NormalizeSupportWithChildren.h"

float NormalizeSupportWithChildren::getAbsoluteSupport(const string& node) {
    float result = getAbsoluteSelfSupport(node);
    for (const auto& child: getChildren(node)) {
        result += getAbsoluteSupport(child);
    }
    return result;
}