#include "ParallelExplicitGraph.h"

#include "ParallelJobsExecutor.h"
#include "../util.h"
#include "../ProgressDisplay.h"

vector<shared_ptr<AdjGraph>> parallelMapNodes(const AdjGraph& source, function<void(AdjGraph&, uint)> mapper) {
    int n = (int)source.size();
    ParallelJobsExecutor<AdjGraph> executor(n);
    return executor.executeParallel([&]() {
        return make_shared<AdjGraph>(n);
    }, [&](AdjGraph& res, int node) {
        mapper(res, node);
    });
    // idea:
    //   start one thread per cpu
    //   add all node indices to a task queue (implicit?)
    //   all worker threads create their own result shared_ptr
    //   each pulls node jobs and calls mapper with their own result graph and that job
    //   return all the #numcpu result graphs
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
    int n = (int)target.size();
    ParallelJobsExecutor<void> executor(n);
    executor.executeParallel([&](int n1) {
        for (const auto& changePtr: changesToApply) {
            AdjGraph& change = *changePtr;
            for (const auto& [n2, w]: change[n1]) {
                if (n1 != n2) {
                    addI(target, n1, n2, w);
                    addI(target, n2, n1, w);
                }
            }
        }
    });
    // idea:
    //   start one thread per cpu
    //   add all node indices to a task queue (implicit?)
    //   workers take job node index, read connections of that node from all input graphs and write to target
}
