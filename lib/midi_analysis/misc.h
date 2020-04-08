#ifndef PROJECT_MISC_H
#define PROJECT_MISC_H

#include <ostream>
#include "analysis.h"
#include "WeightedBipartiteGraph.h"
#include "MidiEvent.h"

void PrintGraphViz(const WeightedBipartiteGraph<MidiChar>& g, std::ostream& o);
SimpleMidiEvent fromMidiEvent(const smf::MidiEvent& mevt);
std::string pitchToNote(uint8_t pitch);
std::ostream& operator<<(std::ostream& o, const MidiChar& ch);
void printMidiEventList(const std::vector<SimpleMidiEvent>& evtlist);
MidiString loadMidiString(const std::string& path);

#endif //PROJECT_MISC_H
