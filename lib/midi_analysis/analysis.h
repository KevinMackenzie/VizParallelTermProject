#ifndef PROJECT_ANALYSIS_H
#define PROJECT_ANALYSIS_H

#include <cstdint>
#include <vector>
#include "WeightedBipartiteGraph.h"

// Encodes "notes" as they are played and released
struct SimpleMidiEvent {
    uint32_t onset;
    uint16_t duration;
    uint8_t pitch;
    uint8_t velocity;

    SimpleMidiEvent() : onset(0), duration(0), pitch(0), velocity(0) {}
};

using MidiString = std::vector<SimpleMidiEvent>;
struct SimpleMidiEventList {
    MidiString events;
    std::vector<float> offsetFreq;
};
inline uint32_t getMidiStringLength(const MidiString& m) { return m.back().onset + m.back().duration - m.front().onset; }

WeightedBipartiteGraph<SimpleMidiEvent> editDistance(const MidiString &ref, const MidiString &inp);
WeightedBipartiteGraph<SimpleMidiEvent> editDistanceDiagonal(const MidiString &ref, const MidiString &inp, bool parallel);

// for CC's with the same pitch and an equal number of L/R nodes, cuts non-in-order pairwise edges
void cutVestigialEdges(WeightedBipartiteGraph<SimpleMidiEvent>& g);

// Filters onset frequency over a string centered over each onset within a given window
std::vector<float> filterOnsetFrequency(const MidiString & str, uint32_t window);
// Filters inter-onset-intervals for the "R" side of the graph when compared against the "L" side of the graph
std::vector<float> filterTimeStretch(const WeightedBipartiteGraph<SimpleMidiEvent> &g, const std::vector<float> &lTempo,
                                     const std::vector<float> &rTempo, uint32_t window);
float weight_func(const SimpleMidiEvent &rch, const SimpleMidiEvent &ich);

#endif //PROJECT_ANALYSIS_H
