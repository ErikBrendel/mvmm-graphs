
#include <vector>
#include <functional>
#include <memory>

using namespace std;

using AdjGraph = vector<unordered_map<uint, double>>;

/**
 * @param mapper applied once per node, on an empty result graph or the result of a previous mapping step
 * @return maybe multiple graphs, maybe just one, at max one per source node
 */
vector<shared_ptr<AdjGraph>> parallelMapNodes(const AdjGraph& source, function<void(AdjGraph& target, uint node)> mapper);

void parallelMergeGraphs(AdjGraph& target, const vector<shared_ptr<AdjGraph>>& changesToApply);
