#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
namespace raven
{
namespace graph
{
using namespace boost;

/** find path with fewest hops
    @param[out] path vector of node indices on path with fewest hops
    @param[in]  start index of node
    @param[in] end index of node
    @param[in] g graph
*/
template <class Graph>
void shortestPathInMaze(
    std::vector<int>& path,
    int start,
    int end,
    Graph& g )
{
    std::vector<int> pred( num_vertices(g));
    pred[start] = start;
    breadth_first_search(
        g,
        start,
        boost::visitor(
            boost::make_bfs_visitor(
                boost::record_predecessors(
                    pred.data(),
                    boost::on_tree_edge()))));

    path.clear();
    int v = end;
    path.push_back( v );
    while( v != start )
    {
        v = pred[v];
        path.push_back( v );
    }

    std::reverse(
        path.begin(),
        path.end() );
}
}
}


