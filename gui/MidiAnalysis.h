#ifndef PROJECT_MIDIANALYSIS_H
#define PROJECT_MIDIANALYSIS_H

#include <optional>
#include <MidiFile.h>
#include <Analysis.h>

// The "Document" of the program
class MidiAnalysis {
public:
    // TODO: this struct may be better put somewhere in the lib
    struct out_data {
        WeightedBipartiteGraph<SimpleMidiEvent> mapping;
        SimpleMidiEventList refList;
        SimpleMidiEventList inpList;
        std::vector<float> refToInpTimeStretch;
    };

    MidiAnalysis() = default;

    void setReference(const MidiString &ref);

    void setInput(const MidiString &inp);

    auto &getReference()  { return reference; }

    auto &getInput()  { return input; }

    MidiString getInputRefScaled() const;

    const auto getAnalysisResults() const { return odata; }

    void Analyze();

    void setTempoWindow(unsigned int wnd) { tempoWindow = wnd; UpdateTempoFilters(); }
    void setStretchWindow(unsigned int wnd) { stretchWindow = wnd; UpdateTempoFilters(); }
    auto getTempoWindow() const { return tempoWindow; }
    auto getStretchWindow() const { return stretchWindow; }

private:

    void UpdateTempoFilters();
    std::optional<MidiString> reference;
    std::optional<MidiString> input;

    unsigned int tempoWindow = 100000;
    unsigned int stretchWindow = 10000;

    std::optional<out_data> odata;
    // TODO: eventually stuff like tempo-tracking
};


#endif //PROJECT_MIDIANALYSIS_H
