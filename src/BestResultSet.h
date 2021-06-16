#pragma once

#include <unordered_set>
#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullVertex.h>

#include "util.h"

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

    vector<pair<vector<double>, UserData>> getBest(const vector<double>& dimWeights);
    const vector<pair<vector<double>, UserData>>& getAllData() const;

    void trim();
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
vector<pair<vector<double>, UserData>> BestResultSet<UserData>::getBest(const vector<double>& dimWeights) {
    sort(all(data), [&](const pair<vector<double>, UserData>& a, const pair<vector<double>, UserData>& b) {
        double agg = 0;
        rep(d, dimensionCount) {
            agg += dimWeights[d] * (a.first[d] - b.first[d]);
        }
        return agg < 0;
    });
    vector<pair<vector<double>, UserData>> result;
    result.reserve(resultKeepSize);
    rep(i, resultKeepSize) {
        result.push_back(data[i]);
    }
    return result;
}

template<typename UserData>
const vector<pair<vector<double>, UserData>>& BestResultSet<UserData>::getAllData() const {
    return data;
}


template<typename UserData>
void BestResultSet<UserData>::trim() {
    if (data.size() < resultKeepSize || data.size() < 10) return;

    try {
        cout << "Trying to trim!" << endl;
        shuffle(all(data), my_brs_random_engine);
        cout << "shuffled!" << endl;
        unordered_set<int> hullPoints;
        vector<pair<vector<double>, int>> dataCoordinatesIndices;
        dataCoordinatesIndices.reserve(data.size());
        rep(ind, data.size()) {
            dataCoordinatesIndices.emplace_back(data[ind].first, ind);
        }
        cout << "dataCoordinatesIndices created: " << dataCoordinatesIndices.size() << endl;

        vector<vector<double>> hullData;
        rep(_it, resultKeepSize) {
            cout << "Trimming loop: " << _it << endl;
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
                    hullData.clear();
                }
            }
            int dimensions = dataCoordinatesIndices.front().first.size();
            cout << "dimensions: " << dimensions << endl;
            if (dimensions < 2) {
                return; // degenerate nodes? just don't trim...
            }
            if (hullData.empty()) {
                cout << "creating hull data" << endl;
                // add fake points to limit the hull to our quadrant only
                rep(_p, dimensions + 1) {
                    vector<double> corner(dimensions, 1);
                    hullData.push_back(corner);
                }
                for (const auto& [x, ind] : dataCoordinatesIndices) {
                    hullData.push_back(x);
                }
                cout << "hull data created: " << hullData.size() << endl;
            }
            // update outer points
            vector<double> minValues(dimensions, 9999);
            for (const auto& [coords, ind]: dataCoordinatesIndices) {
                rep(d, dimensions) {
                    minValues[d] = min(minValues[d], coords[d] + 0.000001);
                }
            }
            auto fakePointCount = dimensions + 1;
            rep(d, dimensions) {
                hullData[d][d] = minValues[d];
            }
            cout << "updated outer points" << endl;
            // do the hull
            vector<double> pointData;
            pointData.reserve(dimensions * hullData.size());
            for (const auto& p: hullData) {
                rep(d, dimensions) {
                    pointData.push_back(p[d]);
                }
            }
            cout << "got pointData: " << pointData.size() << endl;
            //Qhull(const char *inputComment2, int pointDimension, int pointCount, const double *pointCoordinates, const char *qhullCommand2);
            orgQhull::Qhull hull("", dimensions, hullData.size(), pointData.data(), ""); // qhull_options="Qs QJ0.001" ?

            cout << "qhull done! " << hull.vertexCount() << endl;

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
            cout << "recorded hull points: " << hullPoints.size() << endl;
            removeIndices(dataCoordinatesIndices, actualIndices);
            rep(i, actualIndices.size()) actualIndices[i] += fakePointCount; // shift indices to properly operate in hullData array
            removeIndices(hullData, actualIndices);
            cout << "indices cleared: " << dataCoordinatesIndices.size() << endl;
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
        cerr << "QHull crashed! No trimming will be performed: " << e.what() << endl;
    } catch (...) {
        auto e = current_exception();
        cerr << "QHull crashed with unknown exception! No trimming will be performed." << endl;
    }
}

