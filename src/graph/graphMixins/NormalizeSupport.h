#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

using namespace std;

class NormalizeSupport {
private:
    unordered_map<string, double> normalizedSupportPerNodeCache = {};
    pair<double, double> medianMaximumCache = {0, 0};

public:
    virtual double getAbsoluteSupport(const string& node) = 0;
    virtual const vector<string>& getNodeSet() = 0;

    virtual double getNormalizedSupport(const string& node);

private:
    pair<double, double> getAbsoluteSupportMedianAndMax();
};
