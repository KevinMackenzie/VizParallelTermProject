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
    uint32_t prev_onset;
    // The instantaneous tempo (notes / time unit) at this event
    float tempo;
};

using MidiString = std::vector<MidiChar>;

WeightedBipartiteGraph<MidiChar> editDistance(const MidiString &ref, const MidiString &inp);
WeightedBipartiteGraph<MidiChar> editDistanceDiagonal(const MidiString &ref, const MidiString &inp, bool parallel);

void filterTempo(MidiString& str);
float weight_func(const MidiChar &rch, const MidiChar &ich);

#endif //PROJECT_ANALYSIS_H
