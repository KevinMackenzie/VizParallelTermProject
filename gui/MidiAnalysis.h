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
    };

    MidiAnalysis() = default;

    void setReference(const smf::MidiFile &ref);

    void setInput(const smf::MidiFile &inp);

    std::optional<smf::MidiFile> &getReference()  { return reference; }

    std::optional<smf::MidiFile> &getInput()  { return input; }

    const std::optional<out_data> getAnalysisResults() const { return odata; }

    void Analyze();

private:
    std::optional<smf::MidiFile> reference;
    std::optional<smf::MidiFile> input;

    std::optional<out_data> odata;
    // TODO: eventually stuff like tempo-tracking
};


#endif //PROJECT_MIDIANALYSIS_H
