#include <utility>

#include "MidiAnalysis.h"

void MidiAnalysis::Analyze() {
    if (!reference || !input)
        return;

    odata = out_data{};

    odata->refList = convertMidiFile(*reference);
    odata->inpList = convertMidiFile(*input);

    auto refStr = odata->refList.events;
    auto inpStr = odata->inpList.events;

    // TODO: this is to prevent excessive load times on large examples
    if (refStr.size() > 500) {
        refStr.resize(500);
    }
    if (inpStr.size() > 500) {
        inpStr.resize(500);
    }
    auto m = editDistanceDiagonal(refStr, inpStr, true);
    cutVestigialEdges(m);

    odata->mapping = WeightedBipartiteGraph<SimpleMidiEvent>(&odata->refList.events, &odata->inpList.events);
    for (size_t i = 0; i < odata->refList.events.size(); ++i) {
        for (auto r : m.GetLNodeEdges(i)) {
            odata->mapping.AddEdge(i, r.to, r.weight);
        }
    }
    UpdateTempoFilters();
}

void MidiAnalysis::UpdateTempoFilters() {
    if (!odata)
        return;
    odata->refList.offsetFreq = filterOnsetFrequency(odata->refList.events, tempoWindow);
    odata->inpList.offsetFreq = filterOnsetFrequency(odata->inpList.events, tempoWindow);
    odata->refToInpTimeStretch = filterTimeStretch(odata->mapping, odata->refList.offsetFreq, odata->inpList.offsetFreq, stretchWindow);
}

void MidiAnalysis::setInput(const smf::MidiFile &inp) {
    input = inp;
    odata.reset();
}

void MidiAnalysis::setReference(const smf::MidiFile &ref) {
    reference = ref;
    odata.reset();
}

