#pragma once

#include "NormalizeSupport.h"

class NormalizeSupportWithChildren : public NormalizeSupport {
private:
    unordered_map<string, float> absoluteSupportPerNodeCache = {};

public:
    virtual const vector<string>& getChildren(const string& node) = 0;
    virtual float getAbsoluteSelfSupport(const string& node) = 0;

    float getAbsoluteSupport(const string& node) override;
};
