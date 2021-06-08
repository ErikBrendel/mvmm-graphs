#pragma once

#include "NormalizeSupport.h"

class NormalizeSupportWithChildren : public NormalizeSupport {
public:
    virtual vector<string> getChildren(const string& node) = 0;
    virtual float getAbsoluteSelfSupport(const string& node) = 0;

    float getAbsoluteSupport(const string& node) override;
};
