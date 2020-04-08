#include <cmath>

template<typename T>
void WeightedBipartiteGraph<T>::AddEdge(size_t l, size_t r, float w) {
  auto it = tfind(ltr[l], r);
  if (it != ltr[l].end())  // Prevent duplicates (not multigraph)
    return;
  ltr[l].push_back({r, w});
  rtl[r].push_back({l, w});
  total_weight += w;
}


template<typename T>
float WeightedBipartiteGraph<T>::GetEdgeWeight(size_t l, size_t r) const {
  auto it = std::find(ltr[l], r);
  if (it != ltr[l].end()) {
    return it->weight;
  }
  return INFINITY;
}

template<typename T>
void WeightedBipartiteGraph<T>::RemoveEdge(size_t l, size_t r) {
  // Although this is linear wrt the size of the vectors,
  //  we expect that adjacency will be small
  auto it0 = tfind(ltr[l], r);
  if (it0 != ltr[l].end()) {
    total_weight -= it0->weight;
    ltr[l].erase(it0);
  }
  auto it1 = tfind(rtl[r], l);
  if (it1 != rtl[r].end()) {
    rtl[r].erase(it1);
  }
}

template<typename T>
std::ostream& operator<<(std::ostream& o, const WeightedBipartiteGraph<T>& g) {
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
