cmake_minimum_required(VERSION 3.17)
project(coupling_graphs)

set(CMAKE_CXX_STANDARD 17)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(coupling_graphs
        src/errorHandler.cpp
        src/main.cpp
        src/util.cpp
        src/graph/CouplingGraph.cpp
        src/graph/graphMixins/NormalizeSupport.cpp
        src/graph/NodeSetCouplingGraph.cpp
        src/graph/graphMixins/NormalizeSupportWithChildren.cpp
        src/graph/graphMixins/NormalizeCouplingWithChildren.cpp
        src/graph/ExplicitCouplingGraph.cpp
        src/graph/SimilarityCouplingGraph.cpp
        src/graph/ModuleDistanceCouplingGraph.cpp
        src/graph/WeightCombinedGraph.cpp
        src/graph/ResultCachedGraph.cpp
        src/DataStorage.cpp
        src/CommandExecutor.cpp
        src/ProgressDisplay.cpp
        src/BestResultSet.h
        src/disagreementAnalysis.cpp
        src/timsort.hpp
        src/parallel/ParallelExplicitGraph.cpp
        src/parallel/ParallelJobsExecutor.cpp
        )


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Qhull REQUIRED)
find_package(OpenMP)
target_link_libraries(coupling_graphs PRIVATE Threads::Threads stdc++fs qhullcpp qhull_r PUBLIC OpenMP::OpenMP_CXX)
