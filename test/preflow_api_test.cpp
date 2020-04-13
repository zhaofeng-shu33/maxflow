#include <gtest/gtest.h>
#include "Maxflow/graph_loader.h"
#include "Maxflow/algorithms/parallel/parallel_push_relabel.h"

TEST(PREFLOW, CONSTRUCT) {
    typedef uint32_t T;
    typedef uint64_t U;
    std::vector<std::tuple<T,T,U>> edge_list;
    T vertex_cnt = 6;
    T edge_cnt = 8;
    edge_list.push_back(std::make_tuple(0, 1, 15));
    edge_list.push_back(std::make_tuple(1, 2, 12));
    edge_list.push_back(std::make_tuple(0, 3, 4));
    edge_list.push_back(std::make_tuple(3, 4, 10));
    edge_list.push_back(std::make_tuple(2, 3, 3));
    edge_list.push_back(std::make_tuple(4, 1, 5));
    edge_list.push_back(std::make_tuple(2, 5, 7));
    edge_list.push_back(std::make_tuple(4, 5, 10));

    std::vector<std::vector<cached_edge<T, U>>> graph = load_graph(
        edge_list, vertex_cnt, edge_cnt);
    T source = 0, sink = 5;
    std::size_t thread_count = 2;
    parallel_push_relabel::max_flow_instance<std::vector, T, U> solver ( std::move ( graph ),
        source, sink, thread_count);
    U res = solver . find_max_flow ();
    ASSERT_EQ(res, 14);
}