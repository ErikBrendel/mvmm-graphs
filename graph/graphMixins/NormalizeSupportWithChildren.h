#pragma once

#include <unordered_set>
#include "NormalizeSupport.h"

class NormalizeSupportWithChildren : public NormalizeSupport {
private:
    unordered_map<string, float> absoluteSupportPerNodeCache = {};

public:
    virtual const unordered_set <string>& getChildren(const string& node) = 0;
    virtual float getAbsoluteSelfSupport(const string& node) = 0;

    float getAbsoluteSupport(const string& node) override;
};
