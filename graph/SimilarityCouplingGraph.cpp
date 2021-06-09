#include "SimilarityCouplingGraph.h"
#include "../util.h"
#include <iostream>

using namespace std;

SimilarityCouplingGraph::SimilarityCouplingGraph(const string& name) : CouplingGraph(name) {}

const vector<string>& SimilarityCouplingGraph::getNodeSet() {
    return nodes;
}

void SimilarityCouplingGraph::addNode(const string& node, const vector<float>& coords, float support) {
    nodes.push_back(node);
    this->coords[node] = coords;
    this->support[node] = support;

    // normalize coords so that they sum to one - needed for jensen shannon distance
    float coordSum = 0;
    rep(i, coords.size()) {
        coordSum += this->coords[node][i];
    }
    rep(i, coords.size()) {
        this->coords[node][i] /= coordSum;
    }
}

float SimilarityCouplingGraph::getSupport(const string& node) {
    auto found = support.find(node);
    return found == support.end() ? 0 : found->second;
}

float SimilarityCouplingGraph::getAbsoluteSupport(const string& node) {
    return getSupport(node);
}

float rel_entr(float x, float y) {
    if (x > 0 && y != 0) {
        return x * log(x / y);
    } else {
        return 0;
    }
}

float jensenShannonArraySimilarity(const vector<float>& a, const vector<float>& b) {
    // from https://github.com/scipy/scipy/blob/master/scipy/spatial/distance.py#L1288
    vector<float> m(a.size());
    rep(i, a.size()) {
        m[i] = (a[i] + b[i]) / 2.0f;
    }
    float js = 0;
    rep(i, a.size()) {
        js += rel_entr(a[i], m[i]);
        js += rel_entr(b[i], m[i]);
    }
    static const float divisor = (float)log(2) * 2;
    return sqrt(js / divisor);
}

float SimilarityCouplingGraph::getNormalizedCoupling(const string& a, const string& b) {
    auto coordsA = coords.find(a);
    if (coordsA == coords.end()) return 0;
    auto coordsB = coords.find(b);
    if (coordsB == coords.end()) return 0;

    return jensenShannonArraySimilarity(coordsA->second, coordsB->second);
}

void SimilarityCouplingGraph::plaintextContent(ostream& out) {
    // one line per node: name, support, weights...
    rep(i, nodes.size()) {
        if (i != 0) out << "\n";
        out << nodes[i] << "," << support[nodes[i]];
        for (const auto& c: coords[nodes[i]]) {
            out << "," << c;
        }
    }
    out << endl;
}

void SimilarityCouplingGraph::printStatistics() {
    plaintextContent(cout);
}

float SimilarityCouplingGraph::getNormalizedSupport(const string& node) {
    return NormalizeSupport::getNormalizedSupport(node);
}
