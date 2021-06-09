#pragma once

#include <unordered_set>
#include "NormalizeSupport.h"

class NormalizeSupportWithChildren : public NormalizeSupport {
private:
    unordered_map<string, double> absoluteSupportPerNodeCache = {};

public:
    virtual const unordered_set <string>& getChildren(const string& node) = 0;
    virtual double getAbsoluteSelfSupport(const string& node) = 0;

    double getAbsoluteSupport(const string& node) override;
};
