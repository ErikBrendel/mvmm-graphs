#include <memory>

#include "util.h"
#include "graph/CouplingGraph.h"
#include "ProgressDisplay.h"
#include "BestResultSet.h"

// coords data: pattern_match_score_data, -support
// user data: node1, node2, (normalized_coupling_values, support),
using BrsUserData = tuple<string, string, vector<double>>;

void analyzePair(
        const string& node1, const string& node2,
        const vector<shared_ptr<CouplingGraph>>& graphs,
        const vector<vector<double>>& patterns,
        vector<shared_ptr<BestResultSet<BrsUserData>>>& results
);

vector<shared_ptr<BestResultSet<BrsUserData>>> analyzeDisagreements(
        const vector<string>& nodeSet,
        const vector<shared_ptr<CouplingGraph>>& graphs,
        const vector<vector<double>>& patterns,
        int resultSize
);