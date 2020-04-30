#include "AdjacencyList.h"

AdjacencyList::AdjacencyList(IndexType from_size) : data(from_size),
                                                    num_orphans(from_size) {}

bool AdjacencyList::AddEdge(IndexType from, IndexType to, float weight) {
    return data[from].Add(to, weight);
}

const AdjacencyData &AdjacencyList::GetEdges(IndexType from) const {
    return data[from];
}

void AdjacencyList::RemoveEdge(IndexType from, IndexType to) {
    auto &a = data[from];
    a.Remove(to);
    if (a.size == 0) ++num_orphans;
}

bool AdjacencyData::Remove(IndexType to) {
    IndexType i;
    for (i = 0; i < size; ++i) if (to_list[i].to == to) break;
    if (i == size) return false;
    for (; i < size - 1; ++i) to_list[i] = to_list[i + 1];
    --size;
    return true;
}

bool AdjacencyData::Add(IndexType to, float weight) {
    if (size == MAX_CONNECTIVITY) return false;
    auto& r = to_list[size++];
    r.weight = weight;
    r.to = to;
    return true;
}
