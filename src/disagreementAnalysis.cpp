#include "disagreementAnalysis.h"

using namespace std;


static const double MIN_SUPPORT_VALUE = 0.000001; // how much relative support a result needs to not be discarded

void analyzePairSingleDirection(
        const string& a, const string& b,
        const vector<shared_ptr<CouplingGraph>>& graphs,
        const vector<vector<double>>& patterns,
        const vector<double>& supportValues,
        vector<shared_ptr<BestResultSet<BrsUserData>>>& results
) {
    vector<double> normalizedCouplingValues;
    for (const auto& g: graphs) {
        normalizedCouplingValues.push_back(g->getNormalizedCoupling(a, b));
    }
    rep(p, patterns.size()) {
        vector<double> patternMatchScoreData;
        const auto& pattern = patterns[p];
        rep(i, graphs.size()) {
            if (!isnan(pattern[i])) {
                patternMatchScoreData.push_back(abs(pattern[i] - normalizedCouplingValues[i]));
            }
        }
        double support = 1;
        rep(i, graphs.size()) {
            if (!isnan(pattern[i])) {
                support = min(support, supportValues[i]);
            }
        }
        if (support >= MIN_SUPPORT_VALUE) {
            // add support to data vectors
            patternMatchScoreData.push_back(-support); // sort by support descending
            normalizedCouplingValues.push_back(support); // but for display, it should not be inverted
            results[p]->add(patternMatchScoreData, {a, b, normalizedCouplingValues});
        }
    }
}

void analyzePair(
        const string& node1, const string& node2,
        const vector<shared_ptr<CouplingGraph>>& graphs,
        const vector<vector<double>>& patterns,
        vector<shared_ptr<BestResultSet<BrsUserData>>>& results
) {
    if (startsWith(node1, node2) || startsWith(node2, node1)) {
        return; // ignore nodes that are in a parent-child relation
    }
    // for each view: how much support do we have for this node pair (minimum of both node support values)
    vector<double> supportValues;
    for (const auto& g: graphs) {
        auto supp1 = g->getNormalizedSupport(node1);
        auto supp2 = g->getNormalizedSupport(node2);
        supportValues.push_back(min(supp1, supp2));
    }
    analyzePairSingleDirection(node1, node2, graphs, patterns, supportValues, results);
    analyzePairSingleDirection(node2, node1, graphs, patterns, supportValues, results);
}


vector<shared_ptr<BestResultSet<BrsUserData>>> analyzeDisagreements(
        const vector<string>& nodeSet,
        const vector<shared_ptr<CouplingGraph>>& graphs,
        const vector<vector<double>>& patterns,
        int resultSize
) {
    for (const auto& p: patterns) {
        ensure(graphs.size() == p.size(), "Pattern is the wrong size!")
    }

    vector<shared_ptr<BestResultSet<BrsUserData>>> result;
    for (const auto& p: patterns) {
        // one dim for each graph that is used in the pattern + 1 for support
        auto patternDimensions = 1 + count_if(all(p), [](double v) { return !isnan(v); });
        result.push_back(make_shared<BestResultSet<BrsUserData>>(patternDimensions, resultSize));
    }

    ProgressDisplay::init("Analyzing node pairs", square((int) nodeSet.size()));
    rep_all_pairs(n1, n2, nodeSet.size()) {
        ProgressDisplay::update();
        analyzePair(nodeSet[n1], nodeSet[n2], graphs, patterns, result);
    }
    ProgressDisplay::close();

    ProgressDisplay::init("Trimming result sets", (int)result.size());
    for (auto& r: result) {
        ProgressDisplay::update();
        r->trim();
    }
    ProgressDisplay::close();

    return result;
}
