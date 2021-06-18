#pragma once

#include <functional>
#include <memory>

#include "../util.h"

using namespace std;

template <typename ThreadData>
class ParallelJobsExecutor {
private:
    int nextJob;
    int totalJobCount;

public:
    explicit ParallelJobsExecutor(int totalJobCount);
    vector<shared_ptr<ThreadData>> executeParallel(function<shared_ptr<ThreadData>()> dataInit, function<void(ThreadData& data, int jobId)> taskHandler);
};

template <>
class ParallelJobsExecutor<void> {
private:
    int nextJob;
    int totalJobCount;
public:
    explicit ParallelJobsExecutor(int totalJobCount);
    void executeParallel(function<void(int jobId)> taskHandler);
};

template<typename ThreadData>
ParallelJobsExecutor<ThreadData>::ParallelJobsExecutor(int totalJobCount) : nextJob(0), totalJobCount(totalJobCount) {}

ParallelJobsExecutor<void>::ParallelJobsExecutor(int totalJobCount) : nextJob(0), totalJobCount(totalJobCount) {}

template<typename ThreadData>
vector<shared_ptr<ThreadData>>
ParallelJobsExecutor<ThreadData>::executeParallel(function<shared_ptr<ThreadData>()> dataInit,
                                                  function<void(ThreadData&, int)> taskHandler) {
    auto res = dataInit();
    while (nextJob < totalJobCount) {
        taskHandler(*res, nextJob);
        nextJob++;
    }
    return {res};
}


void ParallelJobsExecutor<void>::executeParallel(function<void(int)> taskHandler) {
    while (nextJob < totalJobCount) {
        taskHandler(nextJob);
        nextJob++;
    }
}

