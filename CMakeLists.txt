cmake_minimum_required(VERSION 3.17)
project(coupling_graphs)

set(CMAKE_CXX_STANDARD 17)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(coupling_graphs
        main.cpp util.cpp
        graph/CouplingGraph.cpp
        graph/graphMixins/NormalizeSupport.cpp
        graph/NodeSetCouplingGraph.cpp
        graph/graphMixins/NormalizeSupportWithChildren.cpp
        graph/graphMixins/NormalizeCouplingWithChildren.cpp
        graph/ExplicitCouplingGraph.cpp
        graph/SimilarityCouplingGraph.cpp
        graph/ModuleDistanceCouplingGraph.cpp
        graph/WeightCombinedGraph.cpp
        graph/ResultCachedGraph.cpp
        DataStorage.cpp
        CommandExecutor.cpp
        ProgressDisplay.cpp
        )


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(coupling_graphs PRIVATE Threads::Threads stdc++fs)
