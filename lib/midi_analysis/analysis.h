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

struct SimpleMidiEventList {
    std::vector<SimpleMidiEvent> events;
    // TODO: also have stuff for timing?
};

// A single "char" in the "string" used in edit-distance
struct MidiChar {
    // The current state of the notes last played on the keyboard
    //  NOTE: Attenuation is not factored in yet, and the pedal is not considered
    // SimpleMidiEvent keyboardContext[128];
    // The midi event corresponding to this "char"
    SimpleMidiEvent event;
    // The onset time of the previous note in the string
    // uint32_t prev_onset;
    // The instantaneous tempo (notes / time unit) at this event
    // float tempo;
};

using MidiString = std::vector<MidiChar>;

WeightedBipartiteGraph<MidiChar> editDistance(const MidiString &ref, const MidiString &inp);
WeightedBipartiteGraph<MidiChar> editDistanceDiagonal(const MidiString &ref, const MidiString &inp, bool parallel);

// for CC's with the same pitch and an equal number of L/R nodes, cuts non-in-order pairwise edges
void cutVestigialEdges(WeightedBipartiteGraph<MidiChar>& g);

// Filters onset frequency over a string centered over each onset within a given window
std::vector<float> filterOnsetFrequency(MidiString& str, uint32_t window);
// Filters inter-onset-intervals for the "R" side of the graph when compared against the "L" side of the graph
std::vector<float> filterTimeStretch(const WeightedBipartiteGraph<MidiChar> &g, const std::vector<float> &lTempo,
                                     const std::vector<float> &rTempo, uint32_t window);
float weight_func(const MidiChar &rch, const MidiChar &ich);

#endif //PROJECT_ANALYSIS_H
