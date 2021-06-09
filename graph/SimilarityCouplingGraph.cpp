#include "SimilarityCouplingGraph.h"
#include "../util.h"
#include <iostream>

using namespace std;

SimilarityCouplingGraph::SimilarityCouplingGraph(const string& name) : CouplingGraph(name) {}

const vector<string>& SimilarityCouplingGraph::getNodeSet() {
    return nodes;
}

void SimilarityCouplingGraph::addNode(const string& node, const vector<double>& coords, double support) {
    nodes.push_back(node);
    this->coords[node] = coords;
    this->support[node] = support;
}

double SimilarityCouplingGraph::getSupport(const string& node) {
    auto found = support.find(node);
    return found == support.end() ? 0 : found->second;
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
        m[i] = (aNorm[i] + bNorm[i]) / 2.0f;
    }
    double js = 0;
    rep(i, aNorm.size()) {
        js += rel_entr(aNorm[i], m[i]);
        js += rel_entr(bNorm[i], m[i]);
    }
    static const double divisor = (double)log(2) * 2;
    return sqrt(js / divisor);
}

float DOCUMENT_SIMILARITY_EXP = 8;  // higher = lower equality values, lower = equality values are all closer to 1

double SimilarityCouplingGraph::getNormalizedCoupling(const string& a, const string& b) {
    auto coordsA = coords.find(a);
    if (coordsA == coords.end()) return 0;
    auto coordsB = coords.find(b);
    if (coordsB == coords.end()) return 0;

    double distance = jensenShannonArraySimilarity(coordsA->second, coordsB->second);
    return pow(1 - distance, DOCUMENT_SIMILARITY_EXP);
}

void SimilarityCouplingGraph::plaintextSave(ostream& out) {
    out << "Similarity" << endl;
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
    plaintextSave(cout);
}

double SimilarityCouplingGraph::getNormalizedSupport(const string& node) {
    return NormalizeSupport::getNormalizedSupport(node);
}
