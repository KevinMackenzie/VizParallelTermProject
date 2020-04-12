#include <utility>

#include "MidiAnalysis.h"


MidiAnalysis::MidiAnalysis() {}

void MidiAnalysis::Analyze() {
    if (!reference || !input)
        return;

    auto refList = convertMidiFile(*reference);
    auto refStr = constructMidiString(refList.events);
    auto inpList = convertMidiFile(*input);
    auto inpStr = constructMidiString(inpList.events);

    auto m = editDistance(refStr, inpStr);

    mapping = WeightedBipartiteGraph<SimpleMidiEvent>(refList.events, inpList.events);
    for (size_t i = 0; i < refList.events.size(); ++i) {
        for (auto r : m.GetLNodeEdges(i)) {
            mapping->AddEdge(i, r.to, r.weight);
        }
    }
}

void MidiAnalysis::setInput(const smf::MidiFile &inp) {
    input = inp;
    mapping.reset();
}

void MidiAnalysis::setReference(const smf::MidiFile &ref) {
    reference = ref;
    mapping.reset();
}

