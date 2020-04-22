#include <cmath>
#include <set>
#include <iostream>

template<typename T>
void WeightedBipartiteGraph<T>::AddEdge(size_t l, size_t r, float w) {
    auto it = ltr.GetEdges(l).find(r);
    if (it != ltr.GetEdges(l).end())  // Prevent duplicates (not multigraph)
        return;
    if (ltr.GetEdges(l).full() || rtl.GetEdges(r).full()) {
        // If we are in a situation like this, it is likely we should be orphaning/disconnecting nodes anyway
        // static int i = 0;
        // std::cout << "Failed to add edge (" << l << ", " << r << "): " << i++ << std::endl;
        return;
    }
    ltr.AddEdge(l, r, w);
    rtl.AddEdge(r, l, w);
    total_weight += w;
}


template<typename T>
float WeightedBipartiteGraph<T>::GetEdgeWeight(size_t l, size_t r) const {
    auto it = ltr.GetEdges(l).find(r);
    if (it != ltr.GetEdges(l).end()) {
        return it->weight;
    }
    return INFINITY;
}

template<typename T>
void WeightedBipartiteGraph<T>::RemoveEdge(size_t l, size_t r) {
    // Although this is linear wrt the size of the vectors,
    //  we expect that adjacency will be small
    auto it0 = ltr.GetEdges(l).find(r);
    if (it0 != ltr.GetEdges(l).end()) {
        total_weight -= it0->weight;
        ltr.RemoveEdge(l, r);
        rtl.RemoveEdge(r, l);
    }
}

template<typename T>
connected_component WeightedBipartiteGraph<T>::find_cc(std::vector<size_t> &lq, std::vector<size_t> &rq) const {
    std::set<size_t> lset, rset;
    while (!lq.empty() || !rq.empty()) {
        for (auto l : lq) {
            if (lset.find(l) != lset.end()) continue;
            lset.insert(l);
            for (auto r : ltr.GetEdges(l)) {
                rq.push_back(r.to);
            }
        }
        lq.clear();
        for (auto r : rq) {
            if (rset.find(r) != rset.end()) continue;
            rset.insert(r);
            for (auto l : rtl.GetEdges(r)) {
                lq.push_back(l.to);
            }
        }
        rq.clear();
    }
    return {std::vector<size_t>(lset.begin(), lset.end()), std::vector<size_t>(rset.begin(), rset.end())};
}

template<typename T>
connected_component WeightedBipartiteGraph<T>::GetLCC(size_t l) const {
    std::vector<size_t> lq, rq;
    lq.push_back(l);
    return find_cc(lq, rq);
}

template<typename T>
connected_component WeightedBipartiteGraph<T>::GetRCC(size_t r) const {
    std::vector<size_t> lq, rq;
    rq.push_back(r);
    return find_cc(lq, rq);
}

template<typename T>
std::ostream &operator<<(std::ostream &o, const WeightedBipartiteGraph<T> &g) {
    o << "Weighted Graph:" << std::endl;
    o << "Left Nodes" << std::endl;
    for (size_t i = 0; i < g.GetL().size(); ++i) {
        o << "Node " << i << " (" << g.GetL()[i] << ")" << std::endl;
        for (size_t j = 0; j < g.GetLNodeEdges(i).size(); ++j) {
            o << "  " << g.GetLNodeEdges(i)[j].to << ": " << g.GetLNodeEdges(i)[j].weight << std::endl;
        }
    }
    o << "Right Nodes" << std::endl;
    for (size_t i = 0; i < g.GetR().size(); ++i) {
        o << "Node " << i << " (" << g.GetR()[i] << ")" << std::endl;
        for (size_t j = 0; j < g.GetRNodeEdges(i).size(); ++j) {
            o << "  " << g.GetRNodeEdges(i)[j].to << ": " << g.GetRNodeEdges(i)[j].weight << std::endl;
        }
    }
    return o;
}
