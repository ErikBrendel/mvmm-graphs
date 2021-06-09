#include <iostream>
#include "NormalizeSupportWithChildren.h"

float NormalizeSupportWithChildren::getAbsoluteSupport(const string& node) {
    float result = getAbsoluteSelfSupport(node);
    std::cout << "getAbsoluteSupport " << node << endl;
    for (const auto& child: getChildren(node)) {
        result += getAbsoluteSupport(child);
    }
    return result;
}