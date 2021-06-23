#include "ParallelJobsExecutor.h"

ParallelJobsExecutor::ParallelJobsExecutor(int totalJobCount) : nextJob(0), totalJobCount(totalJobCount) {}

void ParallelJobsExecutor::executeParallel(function<void(int)> taskHandler) {
    int threadCount = getThreadCount();

    vector<thread> workerThreads;
    workerThreads.reserve(threadCount);
    rep(th, threadCount) {
        workerThreads.emplace_back(&ParallelJobsExecutor::handleTasksParallelVoid, this, taskHandler);
    }
    rep(th, threadCount) {
        workerThreads[th].join();
    }
}

int ParallelJobsExecutor::getThreadCount() {
    int threadCount = 2;//min(totalJobCount, (int)thread::hardware_concurrency() - 1);
    cout << "Parallel jobs: " << threadCount << endl;
    return threadCount;
}

void ParallelJobsExecutor::handleTasksParallelVoid(function<void(int)> taskHandler) {
    while (true) {
        int myJob = nextJob++;
        if (myJob >= totalJobCount) break;
        taskHandler(myJob);
    }
}

