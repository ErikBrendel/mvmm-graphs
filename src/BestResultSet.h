#pragma once

#include <unordered_set>
#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullVertex.h>
#include <iomanip>
#include <sstream>

#include "util.h"
#include "ProgressDisplay.h"
#include "timsort.hpp"
#include "radixsort.cpp"

using namespace std;

#define RANDOM_SEED 42

template <typename UserData>
class BestResultSet {
private:
    int dimensionCount;
    int resultKeepSize;
    vector<pair<vector<double>, UserData>> data;
    int totalAmount;
    mt19937 my_brs_random_engine;

public:
    BestResultSet(int dimensionCount, int resultKeepSize);

    [[nodiscard]] int getDimensions() const { return dimensionCount; }

    void addAll(const vector<pair<vector<double>, UserData>>& newData);
    void add(const pair<vector<double>, UserData>& newData);
    void add(const vector<double>& coords, const UserData& userData);

    vector<pair<vector<double>, UserData>> getBest(const vector<double>& dimWeights, int resultSizeFactor = 1);
    const vector<pair<vector<double>, UserData>>& getAllData() const;

    void trimConvexHulls();
    void trimSampling(int precision, int resultSizeFactor);
    void trimSampling();
    void trimMultiSampling();
};

template<typename UserData>
BestResultSet<UserData>::BestResultSet(int dimensionCount, int resultKeepSize) :
    dimensionCount(dimensionCount), resultKeepSize(resultKeepSize), data(), totalAmount(0), my_brs_random_engine(RANDOM_SEED) {}

template<typename UserData>
void BestResultSet<UserData>::addAll(const vector<pair<vector<double>, UserData>>& newData) {
    for (const auto& datum: newData) {
        data.push_back(datum);
    }
    totalAmount += newData.size();
}

template<typename UserData>
void BestResultSet<UserData>::add(const pair<vector<double>, UserData>& newData) {
    data.push_back(newData);
    totalAmount++;
}

template<typename UserData>
void BestResultSet<UserData>::add(const vector<double>& coords, const UserData& userData) {
    add(make_pair(coords, userData));
}

template<typename UserData>
vector<pair<vector<double>, UserData>> BestResultSet<UserData>::getBest(const vector<double>& dimWeights, int resultSizeFactor) {
    gfx::timsort(all(data), [&](const pair<vector<double>, UserData>& a, const pair<vector<double>, UserData>& b) {
        double agg = 0;
        rep(d, dimensionCount) {
            agg += dimWeights[d] * (a.first[d] - b.first[d]);
        }
        return agg < 0;
    });
    vector<pair<vector<double>, UserData>> result;
    auto resultSize = min(resultKeepSize * resultSizeFactor, (int)data.size());
    result.reserve(resultSize);
    rep(i, resultSize) {
        result.push_back(data[i]);
    }
    return result;
}

template<typename UserData>
const vector<pair<vector<double>, UserData>>& BestResultSet<UserData>::getAllData() const {
    return data;
}


template<typename UserData>
void BestResultSet<UserData>::trimConvexHulls() {
    if (data.size() < resultKeepSize || data.size() < 10) return;

    try {
        shuffle(all(data), my_brs_random_engine);
        unordered_set<int> hullPoints;
        vector<pair<vector<double>, int>> dataCoordinatesIndices;
        dataCoordinatesIndices.reserve(data.size());
        rep(ind, data.size()) {
            dataCoordinatesIndices.emplace_back(data[ind].first, ind);
        }

        rep(_it, resultKeepSize) {
            for (int d = (int)dataCoordinatesIndices.front().first.size() - 1; d >= 0; --d) { // eliminate degenerate dimensions
                auto referenceValue = dataCoordinatesIndices.front().first[d];
                bool allValuesAreClose = true;
                for (const auto& [x, ind]: dataCoordinatesIndices) {
                    if (abs(x[d] - referenceValue) >= 0.000001) {
                        allValuesAreClose = false;
                        break;
                    }
                }
                if (allValuesAreClose) {
                    cout << "Removing degenerate dimension " << d << endl;
                    for (auto& element : dataCoordinatesIndices) {
                        element.first.erase(element.first.begin() + d);
                    }
                }
            }
            int dimensions = (int)dataCoordinatesIndices.front().first.size();
            if (dimensions < 2) {
                return; // degenerate nodes? just don't trim...
            }
            // do the hull
            auto fakePointCount = dimensions + 1;
            vector<double> pointData;
            pointData.reserve(dimensions * (dataCoordinatesIndices.size() + dimensions + 1));
            // add dim+1 fake points to limit the hull to our quadrant only
            rep(_p, dimensions + 1) {
                rep(i, dimensions) pointData.push_back(1); // actual values are set later
            }
            for (const auto& [x, ind] : dataCoordinatesIndices) {
                rep(i, dimensions) pointData.push_back(x[i]);
            }
            // update outer points
            vector<double> minValues(dimensions, 9999);
            for (const auto& [coords, ind]: dataCoordinatesIndices) {
                rep(d, dimensions) {
                    minValues[d] = min(minValues[d], coords[d] + 0.000001);
                }
            }
            rep(d, dimensions) {
                // each of the outer points (d) gets one coordinate (d) updated to the min value
                pointData[d * dimensions + d] = minValues[d];
            }
            //Qhull(const char *inputComment2, int pointDimension, int pointCount, const double *pointCoordinates, const char *qhullCommand2);
            orgQhull::Qhull hull("", dimensions, (int)pointData.size() / dimensions, pointData.data(), ""); // qhull_options="Qs QJ0.001" ?

            vector<int> actualIndices;
            for (const orgQhull::QhullVertex& element: hull.vertexList()) {
                auto i = element.id();
                if (i >= fakePointCount) {
                    actualIndices.push_back(i - fakePointCount);
                }
            }
            for (const auto& ind: actualIndices) {
                hullPoints.insert(ind);
            }
            removeIndices(dataCoordinatesIndices, actualIndices);
            if (dataCoordinatesIndices.size() < dimensions + 2) {
                return; // just abort the whole trimming: all of the data points are important!
            }

        }
        cout << "Reduced result size from " << data.size() << " to " << hullPoints.size() << endl;
        vector<pair<vector<double>, UserData>> newData;
        for (const auto& ind: hullPoints) {
            newData.push_back(data[ind]);
        }
        data.swap(newData);
    } catch (const std::exception &e) {
        string what = e.what();
        if (what.empty()) {
            cerr << "QHull crashed with unknown exception! No trimming will be performed." << endl;
        } else {
            auto whatLines = split(what, '\n');
            if (whatLines.size() > 1) {
                cerr << "QHull crashed! No trimming will be performed: " << whatLines[0] << "... (" << (whatLines.size() - 1) << " more lines)" << endl;
            } else {
                cerr << "QHull crashed! No trimming will be performed: " << what << endl;
            }
        }
    } catch (...) {
        auto e = current_exception();
        cerr << "QHull crashed with unknown exception! No trimming will be performed." << endl;
    }
}

template<typename UserData>
void BestResultSet<UserData>::trimSampling(int precision, int resultSizeFactor) {
    if (data.size() < resultKeepSize || data.size() < 10) {
        cout << "Trimming skipped - not enough data." << endl;
        return;
    }

    unordered_set<pair<vector<double>, UserData>> sampledPoints;
    vector<vector<double>> allWeights = generateOneDistributions<double>(dimensionCount, precision);
    //ProgressDisplay::init("Trimming", (int)allWeights.size());
    for (const auto& weights: allWeights) {
        //ProgressDisplay::update();
        for (const auto& best: getBest(weights, resultSizeFactor)) {
            sampledPoints.insert(best);
        }
    }
    //ProgressDisplay::close();

    cout << "Reduced result size from " << data.size() << " to " << sampledPoints.size() << "." << endl;
    data.clear();
    for (const auto& d: sampledPoints) {
        data.push_back(d);
    }
}

template<typename UserData>
void BestResultSet<UserData>::trimSampling() {
    trimSampling(dimensionCount * 2, 3);
}

template<typename UserData>
void BestResultSet<UserData>::trimMultiSampling() {
    trimSampling(dimensionCount, 20);
    trimSampling(dimensionCount * 2, 3);
}
