#include "MidiEvent.h"
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "WeightedBipartiteGraph.h"
#include "analysis.h"
#include "misc.h"

struct pt {
    size_t r, i;
};

#define REORDER_TIME_THRESH 5000
#define ORPHAN_COST 50
struct MemoVal {
    WeightedBipartiteGraph<SimpleMidiEvent> g;
#ifdef TRACK_PATH
    std::vector<pt> path;
#endif

    float totalWeight(int ref_time, int inp_time) const {
        // count the number of orphans that aren't within the reorder threshold
        // TODO: this would be a LOT faster if we did some bookkeeping instead
        int num_orphans = 0;
        for (size_t i = 0; i < g.GetL().size(); ++i) {
            if (ref_time - g.GetL()[i].onset > REORDER_TIME_THRESH && g.GetLNodeDegree(i) == 0) {
                ++num_orphans;
            }
        }
        for (size_t i = 0; i < g.GetR().size(); ++i) {
            if (inp_time - g.GetR()[i].onset > REORDER_TIME_THRESH && g.GetRNodeDegree(i) == 0) {
                ++num_orphans;
            }
        }
        return g.GetTotalWeight() + num_orphans * ORPHAN_COST;
    }
};

float weight_func(const SimpleMidiEvent &rch, const SimpleMidiEvent &ich) {
    return powf(abs((int)rch.pitch - ich.pitch), 2);
}

void addMinWeightInsert(MemoVal &m, size_t curr_inp_idx, const SimpleMidiEvent &inp_val, const std::vector<SimpleMidiEvent> &ref,
                        size_t curr_ref_idx, uint32_t time_thresh) {
    // Handle the base case
    if (curr_ref_idx == 0)
        return;

    // Find the minimum edge weight
    size_t min_idx = 0;
    int closest_pitch_diff = 10000;
    for (size_t i = curr_ref_idx; i > 0; --i) { // Note: We don't look at the "CURRENT" ref index, only up to
        // Restrict to window
        if (ref[curr_ref_idx-1].onset - ref[i-1].onset > time_thresh) break;

        // The whole point of re-ordering is to match out-of-order pitches, so only consider pitch on re-order
        auto w = abs((int)ref[i-1].pitch - inp_val.pitch);
        if (w < closest_pitch_diff) {
            min_idx = i-1;
            closest_pitch_diff = w;
            if (w == 0) {
                break;
            }
        }
    }
    auto min_weight = weight_func(ref[min_idx], inp_val);

    // Find all higher cost edges that don't orphan nodes if removed
    std::vector<size_t> to_remove;
    for (auto e : m.g.GetLNodeEdges(min_idx)) {
        if (e.weight > min_weight && m.g.GetRNodeDegree(e.to) > 1) {
            to_remove.emplace_back(e.to);
        }
    }
    // Remove them
    for (auto it : to_remove) {
        m.g.RemoveEdge(min_idx, it);
    }

    // Add it to the graph
    m.g.AddEdge(min_idx, curr_inp_idx, min_weight);
}

void addMinWeightDelete(MemoVal &m, size_t curr_ref_idx, const SimpleMidiEvent &ref_val, const std::vector<SimpleMidiEvent> &inp,
                        size_t curr_inp_idx, uint32_t time_thresh) {
    // Handle the base case
    if (curr_inp_idx == 0)
        return;

    // Find the minimum edge weight
    size_t min_idx = 0;
    int closest_pitch_diff = 10000;
    for (size_t i = curr_inp_idx; i > 0; --i) { // Note: only looks at 'prev' and not 'curr'
        // Restrict to window
        if (inp[curr_inp_idx-1].onset - inp[i-1].onset > time_thresh) break;

        // The whole point of re-ordering is to match out-of-order pitches, so only consider pitch on re-order
        auto w = abs((int)ref_val.pitch - inp[i-1].pitch);
        if (w < closest_pitch_diff) {
            min_idx = i-1;
            closest_pitch_diff = w;
            if (w == 0) {
                break;
            }
        }
    }
    auto min_weight = weight_func(ref_val, inp[min_idx]);

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
    // Add it to the graph
    m.g.AddEdge(curr_ref_idx, min_idx, min_weight);
}

void editDistanceTile(const MemoVal& mem_r1_i0, const MemoVal& mem_r0_i1, const MemoVal& mem_r1_i1, MemoVal& mem_r0_i0, const std::vector<SimpleMidiEvent> &ref,
                      const std::vector<SimpleMidiEvent> &inp, size_t ref_idx, size_t inp_idx) {
    auto ref_val = ref[ref_idx - 1];
    auto inp_val = inp[inp_idx - 1];

    // Insertion: Connect new input to existing ref
    MemoVal ref_m1 = mem_r1_i0;
    addMinWeightInsert(ref_m1, inp_idx - 1, inp_val, ref, ref_idx - 1, REORDER_TIME_THRESH);

    // Deletion: Connect new ref to existing input
    MemoVal inp_m1 = mem_r0_i1;
    addMinWeightDelete(inp_m1, ref_idx - 1, ref_val, inp, inp_idx - 1, REORDER_TIME_THRESH);

    // Substitution (or match)
    MemoVal prev = mem_r1_i1;
    addMinWeightInsert(prev, inp_idx - 1, inp_val, ref, ref_idx, REORDER_TIME_THRESH);
    addMinWeightDelete(prev, ref_idx - 1, ref_val, inp, inp_idx, REORDER_TIME_THRESH);

    // Handle cost of insertion / deletion in a delayed fashion by discouraging orphaned nodes past the reorder threshold
    float ins_weight = ref_m1.totalWeight(ref_val.onset, inp_val.onset);
    float del_weight = inp_m1.totalWeight(ref_val.onset, inp_val.onset);
    float mat_weight = prev.totalWeight(ref_val.onset, inp_val.onset);

    // std::cout << "Insert: " << ins_weight << "; Delete: " << del_weight << "; Match: " << mat_weight << std::endl;

    // std::cout << "Comparing " << (int) ref_val.pitch << " and " << (int) inp_val.pitch;
    if (ins_weight < del_weight && ins_weight < mat_weight) {
        // std::cout << "; Chose Insertion";
        mem_r0_i0 = ref_m1;
    } else if (del_weight < mat_weight) {
        // std::cout << "; Chose Deletion";
        mem_r0_i0 = inp_m1;
    } else {
        // std::cout << "; Chose Substitution / Match";
        mem_r0_i0 = prev;
    }
#ifdef TRACK_PATH
    mem_r0_i0.path.push_back({ref_idx, inp_idx});
#endif
    // std::cout << std::endl;
}

#define PARALLEL_THRESHOLD 20

// The naive diagnal line method for the edit-distance algorithm.  Note: This won't parallelize the O(n) operation on each tile
WeightedBipartiteGraph<SimpleMidiEvent> editDistanceDiagonal(const MidiString &ref, const MidiString &inp, bool parallel) {
    if (ref.empty() || inp.empty()) return WeightedBipartiteGraph<SimpleMidiEvent>(&ref, &inp);

    // Use 3 memos and indexes to cycle through the 3 diagonal rows that will be in use during edit-distance
    //  This brings memory usage from n^3 to n^2
    // Note: The memo for any diagonal line will be at most the smallest dimension
    struct { std::vector<MemoVal> m; size_t sz; } memo[3];
    MemoVal blankMemoVal = {WeightedBipartiteGraph<SimpleMidiEvent>(&ref, &inp)};
    memo[0] = { std::vector<MemoVal>(std::min(ref.size(), inp.size()), blankMemoVal), 0};
    memo[1] = memo[0];
    memo[2] = memo[0];
    size_t p1_gen = 1, p2_gen = 0, curr_gen = 2;

    std::vector<pt> idxs;
    for (size_t d_idx = 1; d_idx < ref.size() + inp.size(); ++d_idx) {
        if (d_idx % 50 == 0)
            std::cout << "Running Diagonal " << d_idx << " out of " << ref.size() + inp.size() << std::endl;
        // Generate vector of indexes to process in this diagonal
        idxs.clear();
        idxs.reserve(d_idx);
        pt p;
        p.r = d_idx;
        p.i = 1;
        while (p.r > ref.size()) {
            p.r--;
            p.i++;
        }
        while (p.i <= inp.size() && p.r > 0) {
            idxs.push_back(p);
            p.r--;
            p.i++;
        }
        ssize_t p1, p2;
        if (d_idx <= ref.size()) {
            p1 = -1;
            p2 = -1;
        } else {
            p1 = 0;
            p2 = 1;
        }
        // Update the size of the current generation
        memo[curr_gen].sz = idxs.size();

        if (!parallel || idxs.size() < PARALLEL_THRESHOLD) {
            // Do it in serial
            for (ssize_t i = 0; i < idxs.size(); ++i) {
                MemoVal& r1i0 = (i + p1 < 0) ? blankMemoVal : memo[p1_gen].m[i+p1];
                MemoVal& r0i1 = (i+p1+1 >= memo[p1_gen].sz) ? blankMemoVal : memo[p1_gen].m[i+p1+1];
                MemoVal& r1i1 = (i + p2 < 0 || i+p2 >= memo[p2_gen].sz) ? blankMemoVal : memo[p2_gen].m[i+p2];
                editDistanceTile(r1i0, r0i1, r1i1, memo[curr_gen].m[i], ref, inp, idxs[i].r, idxs[i].i);
            }
        } else {
            // Do it in parallel
#pragma omp parallel for
            for (ssize_t i = 0; i < idxs.size(); ++i) {
                MemoVal& r1i0 = (i + p1 < 0) ? blankMemoVal : memo[p1_gen].m[i+p1];
                MemoVal& r0i1 = (i+p1+1 >= memo[p1_gen].sz) ? blankMemoVal : memo[p1_gen].m[i+p1+1];
                MemoVal& r1i1 = (i + p2 < 0 || i+p2 >= memo[p2_gen].sz) ? blankMemoVal : memo[p2_gen].m[i+p2];
                editDistanceTile(r1i0, r0i1, r1i1, memo[curr_gen].m[i], ref, inp, idxs[i].r, idxs[i].i);
            }
        }
        // Advance the memo data
        curr_gen = (curr_gen + 1) % 3;
        p1_gen = (p1_gen + 1) % 3;
        p2_gen = (p2_gen + 1) % 3;
    }
    if (memo[p1_gen].sz != 1) {
        std::cout << "WEIRD: " << memo[p1_gen].m.size() << std::endl;
    }
    return memo[p1_gen].m[0].g;
}

WeightedBipartiteGraph<SimpleMidiEvent> editDistance(const MidiString &ref, const MidiString &inp) {
    if (ref.empty() || inp.empty()) return WeightedBipartiteGraph<SimpleMidiEvent>(&ref, &inp);
    std::vector<std::vector<MemoVal> > memo(ref.size() + 1,
                                            std::vector<MemoVal>(inp.size() + 1,
                                                                 {WeightedBipartiteGraph<SimpleMidiEvent>(&ref, &inp)}));

    // TODO: this was lost when I deleted some stuff, but here is how to deal with tempo-based mis-alignments:
    //  There are a couple of options.  The beam-search approach (see cmu source) is Ok, but relies on relatively simple
    //    metric structures and is fairly complex.  It is also required as a pre-processing step.
    //  The score-following approach (see other cmu src) is not great, since it requires note sequences to be in-order and
    //    match, but it gave me the idea that we can produce tempo as an output of this algorithm and not as an input.
    //    We can calculate the expected "next" note position in time by projecting where the next note would be if it
    //      was played in the current tempo of the reference, which is just the exponential-weighted average of note
    //      onset frequency.  While this is not good for getting tempo on its own (see first cmu source), it could be
    //      close enough and simple enough for our purposes in this algorithm since we are constantly validating against
    //      a second input.  Once we have an effective mapping, we can calculate tempo differentials on a per-match basis
    //      and filter different kinds of tempo differences.

#ifdef TRACK_PATH
    // Fill the base-case row / column in the memo
    for (size_t i = 0; i < ref.size() + 1; ++i) {
      memo[i][0].path.push_back({i, 0});
    }
    for (size_t i = 0; i < inp.size() + 1; ++i) {
      memo[0][i].path.push_back({0, i});
    }
#endif

    for (size_t ref_idx = 1; ref_idx <= ref.size(); ++ref_idx) {
        for (size_t inp_idx = 1; inp_idx <= inp.size(); ++inp_idx) {
            editDistanceTile(memo[ref_idx-1][inp_idx], memo[ref_idx][inp_idx-1], memo[ref_idx-1][inp_idx-1], memo[ref_idx][inp_idx], ref, inp, ref_idx, inp_idx);
        }
    }
    auto ret = memo.back().back();
    std::cout << "Real Result: " << ret.totalWeight(100'000'000, 100'000'000) << std::endl;
#ifdef TRACK_PATH
    std::cout << "Path: " << std::endl;
    for (auto p : ret.path) {
        std::cout << "(" << p.r << ", " << p.i << ")" << std::endl;
    }
#endif
    return ret.g;
}

void cutVestigialEdges(WeightedBipartiteGraph<SimpleMidiEvent>& g) {
    for (size_t i = 0; i < g.GetL().size(); ++i) {
        auto cc = g.GetLCC(i);
        if (cc.lNodes.size() > 1 && cc.lNodes.size() == cc.rNodes.size()) {
            std::sort(cc.lNodes.begin(), cc.lNodes.end());
            std::sort(cc.rNodes.begin(), cc.rNodes.end());
            for (size_t j = 0; j < cc.lNodes.size(); ++j) {
                for (size_t k = 0; k < cc.rNodes.size(); ++k) {
                    if (j != k) g.RemoveEdge(cc.lNodes[j], cc.rNodes[k]);
                }
            }
        }
    }
}
