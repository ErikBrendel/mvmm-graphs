#include "SimilarityCouplingGraph.h"

#include <iostream>
#include <algorithm>

#include "../util.h"

using namespace std;

SimilarityCouplingGraph::SimilarityCouplingGraph(const string& name) : NodeSetCouplingGraph(name) {}

const vector<string>& SimilarityCouplingGraph::getNodeSet() {
    return nodes;
}

void SimilarityCouplingGraph::addNode(const string& node, const vector<double>& coords, double support) {
    nodes.push_back(node);
    this->coords[node] = coords;
    this->support[node] = support;
    onNodeSetChanged();
}

double SimilarityCouplingGraph::getSupport(const string& node) {
    auto found = support.find(node);
    if (found != support.end()) {
        return found->second;
    }
    const auto& children = getChildren(node);
    if (children.empty()) {
        return 0;
    }
    double supportSum = 0;
    for (const auto& child: children) {
        supportSum += getSupport(child);
    }
    support[node] = supportSum; // TODO is it good to store this "cached value" inside the "ground truth" object as well?
    return supportSum;
}

const vector<double>& SimilarityCouplingGraph::getCoords(const string& node) {
    auto found = coords.find(node);
    if (found != coords.end()) {
        return found->second;
    }
    auto dim = getDimensions();
    vector<double> result(dim, 0);
    double totalChildSupport = 0;
    for (const auto& child: getChildren(node)) {
        double childSupport = getSupport(child);
        if (childSupport != 0) {
            const auto& childCoords = getCoords(child);
            totalChildSupport += childSupport;
            rep(d, dim) result[d] += childCoords[d] * childSupport;
        }
    }
    if (totalChildSupport != 0) {
        rep(d, dim) result[d] /= totalChildSupport;
    }
    coords[node] = result; // TODO is it good to store this "cached value" inside the "ground truth" object as well?
    return coords[node];
}

double SimilarityCouplingGraph::getAbsoluteSupport(const string& node) {
    return getSupport(node);
}

double rel_entr(double x, double y) {
    if (x > 0 && y != 0) {
        return x * log(x / y);
    } else {
        return 0;
    }
}

void normalize(vector<double>& data) {
    double sum = 0;
    rep(i, data.size()) {
        sum += data[i];
    }
    rep(i, data.size()) {
        data[i] /= sum;
    }
}

double jensenShannonArraySimilarity(const vector<double>& a, const vector<double>& b) {
    // from https://github.com/scipy/scipy/blob/master/scipy/spatial/distance.py#L1288
    vector<double> aNorm(a);
    vector<double> bNorm(b);
    normalize(aNorm);
    normalize(bNorm);

    vector<double> m(aNorm.size());
    rep(i, aNorm.size()) {
        m[i] = (aNorm[i] + bNorm[i]) / 2.0;
    }
    double js = 0;
    rep(i, aNorm.size()) {
        js += rel_entr(aNorm[i], m[i]);
        js += rel_entr(bNorm[i], m[i]);
    }
    static const double divisor = (double)log(2) * 2;
    return sqrt(js / divisor);
}

double DOCUMENT_SIMILARITY_EXP = 8;  // higher = lower equality values, lower = equality values are all closer to 1

double SimilarityCouplingGraph::getNormalizedCoupling(const string& a, const string& b) {
    double distance = jensenShannonArraySimilarity(getCoords(a), getCoords(b));
    if (isnan(distance)) return 0;
    return pow(1 - distance, DOCUMENT_SIMILARITY_EXP);
}

void SimilarityCouplingGraph::plaintextSave(ostream& out) {
    out << "Similarity" << endl;
    // one line per node: name, support, weights...
    rep(i, nodes.size()) {
        if (i != 0) out << endl;
        out << nodes[i] << "," << support[nodes[i]];
        for (const auto& c: coords[nodes[i]]) {
            out << "," << c;
        }
    }
    out << endl;
}

void SimilarityCouplingGraph::plaintextLoad(istream& in) {
    string line;
    while (getline(in, line)) {
        auto parts = split(line, ',');
        ensure(parts.size() >= 3, "Too few parts for a similarity node line!");
        string node = parts[0];
        double nodeSupport = stof(parts[1]);
        vector<double> nodeCoords;
        for (int i = 2; i < parts.size(); ++i) {
            nodeCoords.push_back(stof(parts[i]));
        }
        addNode(node, nodeCoords, nodeSupport);
    }
}

void SimilarityCouplingGraph::printStatistics() {
    auto nodeCount = coords.size();
    if (nodeCount == 0) {
        cout << "Empty SimilarityCouplingGraph!" << endl;
        return;
    }
    auto dim = getDimensions();
    vector<double> minCoord(dim);
    vector<double> maxCoord(dim);
    for (const auto& [key, val]: coords) {
        rep(d, dim) {
            minCoord[d] = min(minCoord[d], val[d]);
            maxCoord[d] = max(maxCoord[d], val[d]);
        }
    }
    cout << "SimilarityCouplingGraph statistics: " << nodeCount << " nodes" << endl;
    cout << "Min Coordinates" << minCoord << endl;
    cout << "Max Coordinates" << maxCoord << endl;
    vector<double> supports;
    supports.reserve(nodeCount);
    for (const auto& [key, val]: support) {
        supports.push_back(val);
    }
    sort(all(supports));
    cout << "Node support values: ";
    printAbbrev(supports);
    cout << ", mean: " << mean(supports) << endl;
}

double SimilarityCouplingGraph::getNormalizedSupport(const string& node) {
    return NormalizeSupport::getNormalizedSupport(node);
}

size_t SimilarityCouplingGraph::getDimensions() {
    if (coords.empty()) {
        return 0;
    } else {
        return coords.begin()->second.size();
    }
}
