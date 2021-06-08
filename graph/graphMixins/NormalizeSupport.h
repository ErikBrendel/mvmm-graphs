#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

using namespace std;

class NormalizeSupport {
private:
    unordered_map<string, float> normalizedSupportPerNodeCache = {};
    pair<float, float> medianMaximumCache = {0, 0};

public:
    virtual float getAbsoluteSupport(const string& node) = 0;
    virtual const vector<string>& getNodeSet() = 0;

    virtual float getNormalizedSupport(const string& node);

private:
    pair<float, float> getAbsoluteSupportMedianAndMax();
};
