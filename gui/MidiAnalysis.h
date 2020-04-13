#ifndef PROJECT_MIDIANALYSIS_H
#define PROJECT_MIDIANALYSIS_H

#include <optional>
#include <MidiFile.h>
#include <Analysis.h>

// The "Document" of the program
class MidiAnalysis {
public:
    MidiAnalysis() = default;

    void setReference(const smf::MidiFile& ref);
    void setInput(const smf::MidiFile& inp);

    const std::optional<smf::MidiFile> &getReference() const { return reference; }
    const std::optional<smf::MidiFile> &getInput() const { return input; }

    const std::optional<WeightedBipartiteGraph<SimpleMidiEvent> > &getMapping() const { return mapping; }

    void Analyze();

private:
    std::optional<smf::MidiFile> reference;
    std::optional<smf::MidiFile> input;

    std::optional<WeightedBipartiteGraph<SimpleMidiEvent> > mapping;
    // TODO: eventually stuff like tempo-tracking
};


#endif //PROJECT_MIDIANALYSIS_H
