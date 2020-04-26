#ifndef PROJECT_MISC_H
#define PROJECT_MISC_H

#include <ostream>
#include "analysis.h"
#include "WeightedBipartiteGraph.h"
#include "MidiEventList.h"
#include "MidiFile.h"

void PrintGraphViz(const WeightedBipartiteGraph<MidiChar>& g, std::ostream& o);
SimpleMidiEvent fromMidiEvent(int offset, const smf::MidiEvent& mevt);
std::string pitchToNote(uint8_t pitch);
std::ostream& operator<<(std::ostream& o, const MidiChar& ch);
void printMidiEventList(const std::vector<SimpleMidiEvent>& evtlist);
std::vector<SimpleMidiEvent> convertMidiEvents(const smf::MidiEventList &evtList);
SimpleMidiEventList convertMidiFile(const smf::MidiFile& mf);
MidiString constructMidiString(std::vector<SimpleMidiEvent> evts);
MidiString loadMidiString(const std::string& path);
bool isBlackKey(int midiIdx);

#endif //PROJECT_MISC_H
