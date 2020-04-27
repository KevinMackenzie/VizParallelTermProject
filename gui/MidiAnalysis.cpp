#include <utility>
#include <algorithm>

#include "MidiAnalysis.h"

void MidiAnalysis::Analyze() {
    if (!reference || !input)
        return;

    odata = out_data{};

    odata->refList = {*reference};
    odata->inpList = {*input};

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
    odata->refToInpTimeStretch = filterTimeStretch(odata->mapping, odata->refList.offsetFreq, odata->inpList.offsetFreq,
                                                   stretchWindow);
}

void MidiAnalysis::setInput(const MidiString &inp) {
    input = inp;
    odata.reset();
}

void MidiAnalysis::setReference(const MidiString &ref) {
    reference = ref;
    odata.reset();
}

MidiString MidiAnalysis::getInputRefScaled() const {
    if (!odata) return MidiString();
    MidiString ret = odata->inpList.events;
    const auto &g = odata->mapping;

    // Integrate time in Ref-space for input for each event
    // TODO: This isn't that good. error accumulation is pretty bad; probably want to use RK4; does that even make sense?
    // float time = 0;
    for (size_t i = 1; i < ret.size(); ++i) {
        const auto &el = g.GetRNodeEdges(i);
        auto ioiR = odata->inpList.events[i].onset - odata->inpList.events[i - 1].onset;
        if (el.size != 1 || ioiR == 0) {
            ret[i].onset = ret[i-1].onset + ioiR;
            continue;
        }

        const auto &e = el.to_list[0];

        // TODO: how to scale duration?
        ret[i].onset = odata->refList.events[e.to].onset;

        // std::cout << "Old: " << ret[i].onset / 100.f << "; " << ret[i].duration / 100.f << std::endl;
        // auto itrTempo = odata->refList.offsetFreq[el.to_list[0].to] / odata->inpList.offsetFreq[i];
        // time = itrTempo * ioiR + time;
        // ret[i].onset = (uint32_t) ceilf(time);
        // ret[i].duration = (uint16_t) ceilf(itrTempo * odata->inpList.events[i].duration);
        // std::cout << itrTempo << ": " << time / 100.f << "; " << ret[i].duration / 100.f << std::endl;
    }
    return ret;
}

