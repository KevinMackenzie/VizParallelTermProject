#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H

#include <vector>
#include <cstdint>
#include <cstring>

typedef uint16_t IndexType;

#define MAX_CONNECTIVITY 8
struct EdgeData {
    float weight;
    IndexType to;
};

class AdjacencyData {
public:
    EdgeData to_list[MAX_CONNECTIVITY];
    // Do this instead of 'char' b/c it will be aligned anyway
    IndexType size;

    AdjacencyData() : size(0) { memset(to_list, 0, sizeof(to_list)); }

    const EdgeData* begin() const { return this->to_list; }

    const EdgeData* end() const { return this->to_list + this->size; }

    const EdgeData* find(IndexType to) const {
        for (IndexType i = 0; i < size; ++i) {
            if (to == to_list[i].to)
                return &to_list[i];
        }
        return to_list + size;
    }
    EdgeData* find(IndexType to) {
        for (IndexType i = 0; i < size; ++i) {
            if (to == to_list[i].to)
                return &to_list[i];
        }
        return to_list + size;
    }

    bool full() const {
        return size == MAX_CONNECTIVITY;
    }

    bool Remove(IndexType to);
    bool Add(IndexType to, float weight);
};

// Half of an adjacency list for a bipartite graph
class AdjacencyList {
private:
    std::vector<AdjacencyData> data;
    IndexType num_orphans;

public:
    AdjacencyList(IndexType from_size);

    bool AddEdge(IndexType from, IndexType to, float weight);

    const AdjacencyData &GetEdges(IndexType from) const;

    void RemoveEdge(IndexType from, IndexType to);

    // Gets the number of "from" nodes with no "to" nodes attached
    IndexType NumOrphans() const { return num_orphans; }
};

#endif
