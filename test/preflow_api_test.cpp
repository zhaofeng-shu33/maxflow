#include <gtest/gtest.h>
#include "Maxflow/graph_loader.h"

TEST(PREFLOW, CONSTRUCT) {
    typedef uint32_t T;
    typedef uint64_t U;
    std::vector<std::tuple<T,T,U>> edge_list;
    T vertex_cnt = 0;
    T edge_cnt = 0;
    std::vector<std::vector<cached_edge<T, U>>> graph = load_graph(
        edge_list, vertex_cnt, edge_cnt);
}