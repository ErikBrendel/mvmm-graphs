cmake_minimum_required(VERSION 3.17)
project(coupling_graphs)

set(CMAKE_CXX_STANDARD 17)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(coupling_graphs main.cpp CouplingGraph.cpp util.cpp
        graphMixins/NormalizeSupport.cpp graphMixins/NormalizeSupport.h
        NodeSetCouplingGraph.cpp NodeSetCouplingGraph.h
        graphMixins/NormalizeSupportWithChildren.cpp graphMixins/NormalizeSupportWithChildren.h
        graphMixins/NormalizeCouplingWithChildren.cpp graphMixins/NormalizeCouplingWithChildren.h
        ExplicitCouplingGraph.cpp ExplicitCouplingGraph.h
        SimilarityCouplingGraph.cpp SimilarityCouplingGraph.h
        ModuleDistanceCouplingGraph.cpp ModuleDistanceCouplingGraph.h WeightCombinedGraph.cpp WeightCombinedGraph.h ResultCachedGraph.cpp ResultCachedGraph.h)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(coupling_graphs PRIVATE Threads::Threads)
