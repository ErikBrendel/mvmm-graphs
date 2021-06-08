#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

using namespace std;

template <typename BASE>
class NormalizeSupport : public BASE {
    static_assert(std::is_base_of<CouplingGraph, BASE>::value, "can only apply mixin to coupling graphs");
private:
    unordered_map<string, float> normalizedSupportPerNodeCache = {};
    pair<float, float> medianMaximumCache = {0, 0};

public:
    virtual float getAbsoluteSupport(const string& node) = 0;
    virtual const vector<string>& getNodeSet() = 0;

    float getNormalizedSupport(const string& node);

private:
    pair<float, float> getAbsoluteSupportMedianAndMax();
};

#include "NormalizeSupport.inl"
