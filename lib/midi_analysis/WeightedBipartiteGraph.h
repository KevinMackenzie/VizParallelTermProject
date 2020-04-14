#ifndef PROJECT_WEIGHTEDBIPARTITEGRAPH_H
#define PROJECT_WEIGHTEDBIPARTITEGRAPH_H

#include <cstdint>
#include <vector>
#include <ostream>

struct connected_component {
    std::vector<size_t> lNodes;
    std::vector<size_t> rNodes;
};
struct edge_type {
    size_t to;
    float weight;
};

// Adjacency list implementation of a bipartite graph
template<typename T>
class WeightedBipartiteGraph {
private:
    // Note: This has very poor locality.  If its an issue,
    //  we can use an in-place vector and move to the heap
    //  only for abnormally large adjacency lists per node
    std::vector<std::vector<edge_type>> ltr;
    std::vector<std::vector<edge_type>> rtl;
    const std::vector<T> *left;
    const std::vector<T> *right;
    float total_weight;

    static std::vector<edge_type>::iterator tfind(std::vector<edge_type> &vec, size_t t) {
        auto it = vec.begin();
        while (it != vec.end() && it->to != t) ++it;
        return it;
    }
    static std::vector<edge_type>::const_iterator tfind(const std::vector<edge_type> &vec, size_t t) {
        auto it = vec.begin();
        while (it != vec.end() && it->to != t) ++it;
        return it;
    }

    connected_component find_cc(std::vector<size_t>& lq, std::vector<size_t>& rq) const;

public:
    WeightedBipartiteGraph() : left(nullptr), right(nullptr), total_weight(0) {}

    WeightedBipartiteGraph(const std::vector<T> *l, const std::vector<T> *r) : left(l), right(r), ltr(l->size()),
                                                                               rtl(r->size()), total_weight(0) {}

    WeightedBipartiteGraph(const WeightedBipartiteGraph<T> &other) : ltr(other.ltr), rtl(other.rtl),
                                                                     left(other.left), right(other.right),
                                                                     total_weight(other.total_weight) {}

    WeightedBipartiteGraph<T> &operator=(const WeightedBipartiteGraph<T> &other) {
        ltr = other.ltr;
        rtl = other.rtl;
        left = other.left;
        right = other.right;
        total_weight = other.total_weight;
        return *this;
    }

    void AddEdge(size_t l, size_t r, float w);

    float GetEdgeWeight(size_t l, size_t r) const;

    float GetTotalWeight() const { return total_weight; }

    const std::vector<T> &GetL() const { return *left; }
    const std::vector<T> &GetR() const { return *right; }

    size_t GetLNodeDegree(size_t l) const { return ltr[l].size(); }
    size_t GetRNodeDegree(size_t r) const { return rtl[r].size(); }

    const std::vector<edge_type> &GetLNodeEdges(size_t l) const { return ltr[l]; }
    const std::vector<edge_type> &GetRNodeEdges(size_t r) const { return rtl[r]; }

    connected_component GetLCC(size_t l) const;
    connected_component GetRCC(size_t r) const;

    void RemoveEdge(size_t l, size_t r);

};

#include "WeightedBipartiteGraph.inl"

#endif //PROJECT_WEIGHTEDBIPARTITEGRAPH_H
