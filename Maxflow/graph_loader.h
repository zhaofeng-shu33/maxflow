//
// Created by Jan Groschaft on 28.10.18.
//

/*
 * Loads max flow instance in DIMACS format from given input stream.
 * This implementation is not optimised and leaves a lot of space for improvement.
 */

#ifndef MAXFLOW_GRAPH_LOADER_H
#define MAXFLOW_GRAPH_LOADER_H

#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cassert>
#include "common_types.h"
#include <queue>
#include <algorithm>
template <typename T, typename U, template <typename, typename> typename EDGE = cached_edge>
std::vector<std::vector<EDGE<T, U>>> load_graph(std::vector<std::tuple<T,T,U>> edge_list,
    T vertex_cnt, T edge_cnt) {
    auto undefined = std::numeric_limits<T>::max ();
    std::vector<EDGE<T, U>> edges;
    edges . resize ( edge_cnt * 2 );
    std::vector<std::unordered_map<T, T>> edge_map( vertex_cnt );
    std::vector<uint32_t> outgoing_edge_cnt ( vertex_cnt );
    std::size_t pos = 0;
    for (std::tuple<T,T,U>& edge_tuple : edge_list)
    {
        T from = std::get<0>(edge_tuple);
        T to = std::get<1>(edge_tuple);
        U cap = std::get<2>(edge_tuple);
        auto it = edge_map[from] . find ( to );
        //handle multi edges
        if ( it != std::end ( edge_map[from] ) )
        {
            edges[it -> second] . r_capacity += cap;
            break;
        }
        it = edge_map[to] . find ( from );
        //handle backward edges
        if ( it != std::end ( edge_map[to] ) )
            edges[it -> second + 1] . r_capacity += cap;
        else
        {
            edges[pos] = EDGE<T, U> { to, cap, undefined };
            edges[pos + 1] = EDGE<T, U> { from, 0, undefined };
            ++outgoing_edge_cnt[from];
            ++outgoing_edge_cnt[to];
            edge_map[from] . emplace ( to, pos );
            pos += 2;
        }
    }
    edge_map . clear ();
    edge_map . shrink_to_fit ();    
    //alloc graph
    std::vector<std::vector<EDGE<T, U>>> graph ( vertex_cnt );
    load_graph_from_ds(graph, edges, outgoing_edge_cnt);
    return graph;
}

template <typename T, typename U, template <typename, typename> typename EDGE = basic_edge>
void load_graph_from_ds (std::vector<std::vector<EDGE<T, U>>>& graph,
    std::vector<EDGE<T, U>> edges,
    std::vector<uint32_t> outgoing_edge_cnt) {
    for ( std::size_t i = 0; i < graph . size (); ++i )
        graph[i] . reserve ( outgoing_edge_cnt[i] );

    //insert forward edges
    for ( std::size_t i = 0; i < edges.size(); i += 2 )
    {
        auto & edge = edges[i];
        auto & reverse_edge = edges[i + 1];

        graph[reverse_edge . dst_vertex] . emplace_back ( edge . dst_vertex,
                    edge . r_capacity, i + 1 );
    }

    auto sizes = std::make_unique<uint32_t[]> ( graph . size () );
    for ( std::size_t i = 0; i < graph . size (); ++i )
        sizes[i] = graph[i] . size ();

    //insert backward edges
    for ( std::size_t i = 0; i < graph . size (); ++i )
    {
        for ( std::size_t k = 0; k < sizes[i]; ++k )
        {
            auto & edge = graph[i][k];
            auto reverse_edge = edges[edge . reverse_edge_index];
            edge . reverse_edge_index = graph[edge . dst_vertex] . size ();
            graph[edge . dst_vertex] . emplace_back ( i, reverse_edge . r_capacity, k );
        }
    }    
}

template <typename T, typename U, template <typename, typename> typename EDGE = basic_edge>
auto load_graph ( std::istream & is )
{
    char type;
    T vertex_cnt, source, sink;
    auto undefined = std::numeric_limits<T>::max ();
    std::vector<EDGE<T, U>> edges;

    std::string line;
    std::stringstream sstr;
    while ( std::getline ( is, line ) )
    {
        sstr . str ( line );
        sstr >> type;
        if ( type == 'p' )
            break;
    }
    std::string problem_type;
    T edge_cnt;
    sstr >> problem_type;
    if ( problem_type != "max" )
        throw std::logic_error ( "Expected max flow problem instance" );
    sstr >> vertex_cnt >> edge_cnt;
    edges . resize ( edge_cnt * 2 );
    auto edge_map = std::vector<std::unordered_map<T, T>> ( vertex_cnt ); //to keep track of edge positions
    std::vector<uint32_t> outgoing_edge_cnt ( vertex_cnt );

    std::size_t pos = 0;
    while ( std::getline ( is, line ) )
    {
        sstr . clear ();
        sstr . str ( line );
        sstr >> type;
        switch ( type )
        {
            case 'a':
            {
                T from, to;
                U cap;
                sstr >> from >> to >> cap;
                --to;
                --from; // convert to zero based indexing
                auto it = edge_map[from] . find ( to );
                //handle multi edges
                if ( it != std::end ( edge_map[from] ) )
                {
                    edges[it -> second] . r_capacity += cap;
                    break;
                }
                it = edge_map[to] . find ( from );
                //handle backward edges
                if ( it != std::end ( edge_map[to] ) )
                    edges[it -> second + 1] . r_capacity += cap;
                else
                {
                    edges[pos] = EDGE<T, U> { to, cap, undefined };
                    edges[pos + 1] = EDGE<T, U> { from, 0, undefined };
                    ++outgoing_edge_cnt[from];
                    ++outgoing_edge_cnt[to];
                    edge_map[from] . emplace ( to, pos );
                    pos += 2;
                }
                break;
            }
            case 'n':
            {
                T vertex;
                char what;
                sstr >> vertex >> what;
                switch ( what )
                {
                    case 's':
                        source = vertex - 1;
                        break;
                    case 't':
                        sink = vertex - 1;
                        break;
                    default:
                        throw std::logic_error ( "Expected source or sink specification" );
                }
                break;
            }
            case 'c':
                break;
            default:
                std::cout << "Warning: unknown statement: " << type << '\n';
                break;
        }
    }
    edge_map . clear ();
    edge_map . shrink_to_fit ();

    //alloc graph
    std::vector<std::vector<EDGE<T, U>>> graph ( vertex_cnt );
    load_graph_from_ds(graph, edges, outgoing_edge_cnt);

    return std::make_tuple ( std::move ( graph ), source, sink );
}

// Set reverse_r_capacity for cached edges used in push-relabel methods.
template <typename T, typename U>
void set_reverse_edge_cap ( std::vector<std::vector<cached_edge<T, U>>> & graph )
{
    for ( auto & vec : graph )
        for ( auto & edge : vec )
            edge . reverse_r_capacity = graph[edge . dst_vertex][edge . reverse_edge_index] . r_capacity;
}


#endif //MAXFLOW_GRAPH_LOADER_H
