#include "MidiEvent.h"
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "WeightedBipartiteGraph.h"
#include "analysis.h"
#include "misc.h"


struct MemoVal {
  WeightedBipartiteGraph<MidiChar> g;
};

float weight_func(const MidiChar &ch0, const MidiChar &ch1) {
  float pitch_comp = abs(ch0.event.pitch - ch1.event.pitch);
  float time_comp = abs((float) ch0.event.onset - ch1.event.onset) / 100.f;
  return pitch_comp + time_comp;
}

void addMinWeightInsert(MemoVal &m, size_t curr_inp_idx, const MidiChar &inp_val, const std::vector<MidiChar> &ref,
                        size_t curr_ref_idx) {
  // Handle the base case
  if (curr_ref_idx == 0)
    return;

  // Find the minimum edge weight
  size_t min_idx = 0;
  float min_weight = INFINITY;
  for (size_t i = 0; i < curr_ref_idx; ++i) { // Note: We don't look at the "CURRENT" ref index, only up to
    float w = weight_func(ref[i], inp_val);
    if (w < min_weight) {
      min_idx = i;
      min_weight = w;
    }
  }
  // Add it to the graph
  m.g.AddEdge(min_idx, curr_inp_idx, min_weight);

  // Find all higher cost edges that don't orphan nodes if removed
  std::vector<size_t> to_remove;
  for (auto it : m.g.GetLNodeEdges(min_idx)) {
    if (it.weight > min_weight && m.g.GetRNodeDegree(it.to) > 1) {
      to_remove.emplace_back(it.to);
    }
  }
  // Remove them
  for (auto it : to_remove) {
    m.g.RemoveEdge(min_idx, it);
  }
}

void addMinWeightDelete(MemoVal &m, size_t curr_ref_idx, const MidiChar &ref_val, const std::vector<MidiChar> &inp,
                        size_t curr_inp_idx) {
  // Handle the base case
  if (curr_inp_idx == 0)
    return;

  // Find the minimum edge weight
  size_t min_idx = 0;
  float min_weight = INFINITY;
  for (size_t i = 0; i < curr_inp_idx; ++i) { // Note: only looks at 'prev' and not 'curr'
    float w = weight_func(ref_val, inp[i]);
    if (w < min_weight) {
      min_idx = i;
      min_weight = w;
    }
  }
  // Add it to the graph
  m.g.AddEdge(curr_ref_idx, min_idx, min_weight);

  // Find all higher cost edges that don't orphan nodes if removed
  std::vector<size_t> to_remove;
  for (auto it : m.g.GetRNodeEdges(min_idx)) {
    if (it.weight > min_weight && m.g.GetLNodeDegree(it.to) > 1) {
      to_remove.emplace_back(it.to);
    }
  }
  // Remove them
  for (auto it : to_remove) {
    m.g.RemoveEdge(it, min_idx);
  }
}

void editDistanceTile(std::vector<std::vector<MemoVal> > &memo, const std::vector<MidiChar> &ref,
                      const std::vector<MidiChar> &inp, size_t ref_idx, size_t inp_idx) {
  auto ref_val = ref[ref_idx - 1];
  auto inp_val = inp[inp_idx - 1];

  // Insertion: Connect new input to existing ref
  MemoVal ref_m1 = memo[ref_idx - 1][inp_idx];
  addMinWeightInsert(ref_m1, inp_idx - 1, inp_val, ref, ref_idx - 1);

  // Deletion: Connect new ref to existing input
  MemoVal inp_m1 = memo[ref_idx][inp_idx - 1];
  addMinWeightDelete(inp_m1, ref_idx - 1, ref_val, inp, inp_idx - 1);

  // Substitution (or match)
  MemoVal prev = memo[ref_idx - 1][inp_idx - 1];
  addMinWeightInsert(prev, inp_idx - 1, inp_val, ref, ref_idx);
  addMinWeightDelete(prev, ref_idx - 1, ref_val, inp, inp_idx);

  // TODO: how do we adjust the weight for the insertion / deletion case?  Or do we event want to?
  // TODO: we probably want to make "orphan" nodes be fairly costly
  float ins_weight = ref_m1.g.GetTotalWeight();
  float del_weight = inp_m1.g.GetTotalWeight();
  float mat_weight = prev.g.GetTotalWeight();

  // std::cout << "Comparing " << (int) ref_val.event.pitch << " and " << (int) inp_val.event.pitch;
  if (ins_weight < del_weight && ins_weight < mat_weight) {
    // std::cout << "; Chose Insertion";
    memo[ref_idx][inp_idx] = ref_m1;
  } else if (del_weight < mat_weight) {
    // std::cout << "; Chose Deletion";
    memo[ref_idx][inp_idx] = inp_m1;
  } else {
    // std::cout << "; Chose Substitution / Match";
    memo[ref_idx][inp_idx] = prev;
  }
  // std::cout << std::endl;
}

struct pt {
  size_t r, i;
};
#define PARALLEL_THRESHOLD 20

// The naive diagnal line method for the edit-distance algorithm.  Note: This won't parallelize the O(n) operation on each tile
WeightedBipartiteGraph<MidiChar> editDistancePDiagnal(const MidiString &ref, const MidiString &inp) {
  if (ref.empty() || inp.empty()) return WeightedBipartiteGraph<MidiChar>(&ref, &inp);
  std::vector<std::vector<MemoVal> > memo(ref.size() + 1,
                                          std::vector<MemoVal>(inp.size() + 1,
                                                               {WeightedBipartiteGraph<MidiChar>(&ref, &inp)}));

  // TODO: handle non-square case
  for (size_t d_idx = 1; d_idx < 2 * ref.size(); ++d_idx) {
    // Generate vector of indexes to process in this diagnal
    std::vector<pt> idxs;
    idxs.reserve(d_idx);
    pt p;
    p.r = d_idx;
    p.i = 1;
    while (p.r > ref.size()) {
      p.r--;
      p.i++;
    }
    while (p.i <= ref.size() && p.r > 0) {
      idxs.push_back(p);
      p.r--;
      p.i++;
    }

    if (idxs.size() < PARALLEL_THRESHOLD) {
      // Do it in serial
      for (auto idx : idxs) {
        editDistanceTile(memo, ref, inp, idx.r, idx.i);
      }
    } else {
      // Do it in parallel
#pragma omp parallel for
      for (size_t i = 0; i < idxs.size(); ++i) {
        editDistanceTile(memo, ref, inp, idxs[i].r, idxs[i].i);
      }
    }
  }
  return memo.back().back().g;
}

WeightedBipartiteGraph<MidiChar> editDistance(const MidiString &ref, const MidiString &inp) {
  if (ref.empty() || inp.empty()) return WeightedBipartiteGraph<MidiChar>(&ref, &inp);
  std::vector<std::vector<MemoVal> > memo(ref.size() + 1,
                                          std::vector<MemoVal>(inp.size() + 1,
                                                               {WeightedBipartiteGraph<MidiChar>(&ref, &inp)}));

  // Fill the base-case row / column in the memo
  // for (size_t i = 0; i < ref.size() + 1; ++i) {
  //   memo[i][0].g =;
  // }
  // for (size_t i = 0; i < inp.size() + 1; ++i) {
  //   memo[0][i].g =
  // }

  for (size_t ref_idx = 1; ref_idx <= ref.size(); ++ref_idx) {
    size_t inp_idx = 1;
    for (size_t inp_idx = 1; inp_idx <= inp.size(); ++inp_idx) {
      editDistanceTile(memo, ref, inp, ref_idx, inp_idx);
    }
  }
  return memo.back().back().g;
}
