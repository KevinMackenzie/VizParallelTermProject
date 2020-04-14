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


float filterWeight(float f) {
    const int base = 2;
    const float pow_div = 0.0001f;
    return powf(base, -f * pow_div);
}

void filterTempo(MidiString &str) {
    const size_t filterSize = 2;
    for (size_t i = filterSize; i < str.size(); ++i) {
        auto myOnset = str[i].event.onset;
        uint32_t time_accum = 0;
        float weight_accum = 0;
        for (size_t j = i; j > i - filterSize; --j) {
            time_accum += str[j].event.onset - str[j - 1].event.onset;
            weight_accum += filterWeight(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
        }
        str[i].tempo = weight_accum / time_accum;
    }

    // Now weight the first few in ... reverse? maybe idk
    for (size_t i = 0; i < filterSize; ++i) {
        str[i].tempo = str[filterSize].tempo;
    }
}

struct MemoVal {
    WeightedBipartiteGraph<MidiChar> g;
    float extra_weight;

    float totalWeight() const { return extra_weight + g.GetTotalWeight(); }
};

float weight_func(const MidiChar &rch, const MidiChar &ich) {
    float pitch_comp = abs(rch.event.pitch - ich.event.pitch);
    float time_comp = 0;
    if (rch.prev_onset != 0 && ich.prev_onset != 0) {
        // This is flawed b/c two notes could be very far apart, but when aligned at the previous onset, they could
        //  have very similar expected onset projections in the new time-space.  I think we to (a) sanity check
        //  that the notes are reasonably close in time or (b) factor absolute time in some numerically.
        // We could mitigate this by reducing the amount of time that the linear scan on each tile is allowed to go
        //  backwards (i.e. we only find the best "match" in a temporal area since re-orderings are likely to only
        //             occur in a relatively small window: 0.5 seconds is a large estimate)
        //  This doesn't completely solve the problem since similar problems in that window are still possible;
        //      Since this is DP, we could increase the cost the further away it is from the reference during the
        //      linear back-track, which will still need to re-order for some inputs hmm...
        // The goal with this part of the weight is to balance the cost of insertion / removal with non-trivial
        //  local onset discrepancies, so we use the nature of edit-distance to take care of large discrepancies
        auto expected_onset_interval = (rch.event.onset - rch.prev_onset) * (rch.tempo / ich.tempo);
        auto expected_onset = ich.prev_onset + expected_onset_interval;
        time_comp = fabsf(expected_onset - ich.event.onset) / 10.f;
        if (pitch_comp == 0)
            std::cout << "Time Diff: " << time_comp << std::endl;
    }
    // float time_comp = abs(ch0.event.onset - ch1.event.onset) / 100.f;
    return pitch_comp + time_comp;
}

void addMinWeightInsert(MemoVal &m, size_t curr_inp_idx, const MidiChar &inp_val, const std::vector<MidiChar> &ref,
                        size_t curr_ref_idx, uint32_t time_thresh) {
    // Handle the base case
    if (curr_ref_idx == 0)
        return;

    // Find the minimum edge weight
    size_t min_idx = 0;
    float min_weight = INFINITY;
    for (size_t i = curr_ref_idx; i > 0; --i) { // Note: We don't look at the "CURRENT" ref index, only up to
        // Restrict to window
        if (ref[i-1].event.onset - inp_val.event.onset > time_thresh && i != curr_ref_idx) break;
        // TODO: maybe additional weight to discourage unnecessary "swapping" of values
        float w = weight_func(ref[i-1], inp_val);
        if (w < min_weight) {
            min_idx = i-1;
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
                        size_t curr_inp_idx, uint32_t time_thresh) {
    // Handle the base case
    if (curr_inp_idx == 0)
        return;

    // Find the minimum edge weight
    size_t min_idx = 0;
    float min_weight = INFINITY;
    for (size_t i = curr_inp_idx; i > 0; --i) { // Note: only looks at 'prev' and not 'curr'
        // Restrict to window
        if (inp[i-1].event.onset - ref_val.event.onset > time_thresh && i != curr_inp_idx) break;
        // TODO: maybe additional weight to discourage unnecessary "swapping" of values
        float w = weight_func(ref_val, inp[i-1]);
        if (w < min_weight) {
            min_idx = i-1;
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
    addMinWeightInsert(ref_m1, inp_idx - 1, inp_val, ref, ref_idx - 1, 500);
    std::cout << ref_m1.g.GetTotalWeight() << std::endl;
    ref_m1.extra_weight += 50;

    // Deletion: Connect new ref to existing input
    MemoVal inp_m1 = memo[ref_idx][inp_idx - 1];
    addMinWeightDelete(inp_m1, ref_idx - 1, ref_val, inp, inp_idx - 1, 500);
    std::cout << inp_m1.g.GetTotalWeight() << std::endl;
    inp_m1.extra_weight += 50;

    // Substitution (or match)
    MemoVal prev = memo[ref_idx - 1][inp_idx - 1];
    std::cout << prev.g.GetTotalWeight() << std::endl;
    addMinWeightInsert(prev, inp_idx - 1, inp_val, ref, ref_idx, 500);
    addMinWeightDelete(prev, ref_idx - 1, ref_val, inp, inp_idx, 500);

    // TODO: how do we adjust the weight for the insertion / deletion case?  Or do we event want to?
    // TODO: we probably want to make "orphan" nodes be fairly costly: maybe proportional to some "importance" value based on dynamic, timing, etc
    // TODO: do we bake this into the graph?
    float ins_weight = ref_m1.totalWeight();
    float del_weight = inp_m1.totalWeight();
    float mat_weight = prev.totalWeight();

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
