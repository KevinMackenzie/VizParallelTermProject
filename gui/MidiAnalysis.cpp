#include <utility>

#include "MidiAnalysis.h"

void MidiAnalysis::Analyze() {
    if (!reference || !input)
        return;

    odata = out_data{};

    odata->refList = convertMidiFile(*reference);
    auto refStr = constructMidiString(odata->refList.events);
    odata->inpList = convertMidiFile(*input);
    auto inpStr = constructMidiString(odata->inpList.events);

    auto m = editDistanceDiagonal(refStr, inpStr, true);

    odata->mapping = WeightedBipartiteGraph<SimpleMidiEvent>(&odata->refList.events, &odata->inpList.events);
    for (size_t i = 0; i < odata->refList.events.size(); ++i) {
        for (auto r : m.GetLNodeEdges(i)) {
            odata->mapping.AddEdge(i, r.to, r.weight);
        }
    }
}

void MidiAnalysis::setInput(const smf::MidiFile &inp) {
    input = inp;
    odata.reset();
}

void MidiAnalysis::setReference(const smf::MidiFile &ref) {
    reference = ref;
    odata.reset();
}

