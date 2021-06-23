#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <atomic>

#include "../util.h"

using namespace std;

class ParallelJobsExecutor {
private:
    atomic_int nextJob;
    int totalJobCount;

public:
    explicit ParallelJobsExecutor(int totalJobCount);
    template <typename ThreadData>
    vector<shared_ptr<ThreadData>> executeParallel(function<shared_ptr<ThreadData>()> dataInit, function<void(ThreadData& data, int jobId)> taskHandler);
    void executeParallel(function<void(int jobId)> taskHandler);

private:
    int getThreadCount();
    template <typename ThreadData>
    void handleTasksParallel(ThreadData& td, function<void(ThreadData&, int)> taskHandler);
    void handleTasksParallelVoid(function<void(int)> taskHandler);
};

template<typename ThreadData>
vector<shared_ptr<ThreadData>> ParallelJobsExecutor::executeParallel(function<shared_ptr<ThreadData>()> dataInit,
                                                  function<void(ThreadData&, int)> taskHandler) {
    int threadCount = getThreadCount();

    vector<shared_ptr<ThreadData>> threadData;
    vector<thread> workerThreads;
    workerThreads.reserve(threadCount);
    rep(th, threadCount) {
        threadData.push_back(dataInit());
        workerThreads.emplace_back(&ParallelJobsExecutor::handleTasksParallel<ThreadData>, this, ref(*threadData.back()), taskHandler);
    }
    for (auto& th: workerThreads) {
        th.join();
    }
    return threadData;
}

template <typename ThreadData>
void ParallelJobsExecutor::handleTasksParallel(ThreadData& td, function<void(ThreadData&, int)> taskHandler) {
    while (true) {
        int myJob = nextJob++;
        if (myJob >= totalJobCount) break;
        taskHandler(td, myJob);
    }
}
