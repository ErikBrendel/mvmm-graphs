#include "ParallelExplicitGraph.h"

#include "../util.h"
#include "../ProgressDisplay.h"

vector<shared_ptr<AdjGraph>> parallelMapNodes(const AdjGraph& source, function<void(AdjGraph&, uint)> mapper) {
    // idea:
    //   start one thread per cpu
    //   add all node indices to a task queue (implicit?)
    //   all worker threads create their own result shared_ptr
    //   each pulls node jobs and calls mapper with their own result graph and that job
    //   return all the #numcpu result graphs

    // guard clause: use this single threaded version if less nodes than cpu cores :D
    int n = (int)source.size();
    auto res = make_shared<AdjGraph>();
    res->resize(n);
    rep(i, n) {
        mapper(*res, i);
    }
    return {res};
}

void addI(AdjGraph& target, uint a, uint b, double delta) {
    auto& edgeList = target[a];
    auto existingConnection = edgeList.find(b);
    if (existingConnection == edgeList.end()) {
        edgeList[b] = delta;
    } else {
        existingConnection->second += delta;
    }
}


void parallelMergeGraphs(AdjGraph& target, const vector<shared_ptr<AdjGraph>>& changesToApply) {
    // idea:
    //   start one thread per cpu
    //   add all node indices to a task queue (implicit?)
    //   workers take job node index, read connections of that node from all input graphs and write to target

    int n = (int)target.size();
    for (const auto& changePtr: changesToApply) {
        AdjGraph& change = *changePtr;
        ProgressDisplay::init("Applying changes", target.size());
        rep(n1, n) {
            ProgressDisplay::update();
            for (const auto& [n2, w]: change[n1]) {
                if (n1 != n2) {
                    addI(target, n1, n2, w);
                    addI(target, n2, n1, w);
                }
            }
        }
        ProgressDisplay::close();
    }
}
