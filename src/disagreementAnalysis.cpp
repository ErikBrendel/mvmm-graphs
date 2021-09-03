#include "disagreementAnalysis.h"

using namespace std;

/**
 * how much relative support a result needs to not be discarded
 */
static const double MIN_SUPPORT_VALUE = 0;
/**
 * how much each coupling value is allowed to deviate from the target pattern value,
 * so that not the whole module pair is discarded immediately.
 * With a value slightly lower than one, we ensure that
 * no "absolutely-not-matching results" will ever be reported.
 */
static const double MAX_PATTERN_COMPONENT_MISMATCH = 0.9999;

void analyzePairSingleDirection(
        const string& a, const string& b,
        const vector<shared_ptr<CouplingGraph>>& graphs,
        const vector<vector<double>>& patterns,
        const vector<double>& supportValues,
        vector<shared_ptr<BestResultSet<BrsUserData>>>& results
) {
    vector<double> normalizedCouplingValues;
    normalizedCouplingValues.reserve(graphs.size());
    for (const auto& g: graphs) {
        normalizedCouplingValues.push_back(g->getNormalizedCoupling(a, b));
    }
    rep(p, patterns.size()) {
        vector<double> patternMismatchData;
        const auto& pattern = patterns[p];
        double support = 1;
        bool allPatternComponentMatchValuesAreValid = true;
        rep(i, graphs.size()) {
            if (!isnan(pattern[i])) {
                double patternComponentMismatch = abs(pattern[i] - normalizedCouplingValues[i]);
                if (patternComponentMismatch > MAX_PATTERN_COMPONENT_MISMATCH) {
                    allPatternComponentMatchValuesAreValid = false;
                    break;
                }
                patternMismatchData.push_back(patternComponentMismatch);
                support = min(support, supportValues[i]);
            }
        }
        if (allPatternComponentMatchValuesAreValid && support > MIN_SUPPORT_VALUE) {
            // add support to data vectors
            patternMismatchData.push_back(1 - support); // sort by support descending
            normalizedCouplingValues.push_back(support); // but for display, it should not be inverted
            results[p]->add(patternMismatchData, {a, b, normalizedCouplingValues});
            normalizedCouplingValues.pop_back(); // remove support again for the next one
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
    ProgressDisplay::showImmediately();

    auto rs = result.size();

    #pragma omp parallel for default(none) shared(rs, result)
    rep(r, rs) {
        // result[r]->trimSampling();
        result[r]->trimMultiSampling();
        // result[r]->trimConvexHulls();
        ProgressDisplay::update();
    }
    ProgressDisplay::close();

    return result;
}
