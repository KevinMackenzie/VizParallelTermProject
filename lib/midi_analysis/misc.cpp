#include <sstream>
#include "misc.h"
#include "MidiFile.h"
#include <sstream>
#include <iostream>
#include <iomanip>

SimpleMidiEvent fromMidiEvent(int offset, const smf::MidiEvent &mevt) {
    SimpleMidiEvent evt;
    evt.duration = mevt.getTickDuration();
    evt.onset = mevt.tick - offset;
    evt.pitch = mevt.getKeyNumber();
    evt.velocity = mevt.getVelocity();
    return evt;
}

std::string pitchToNote(uint8_t pitch) {
    char pitches[][12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    std::stringstream ss;
    ss << pitches[pitch % 12] << -1 + pitch / 12;
    return ss.str();
}

bool isBlackKey(int midiIdx) {
    int pc = midiIdx % 12;
    return (pc < 4 && pc % 2 == 1) || (pc > 4 && pc % 2 == 0);
}

std::ostream &operator<<(std::ostream &o, const SimpleMidiEvent &ch) {
    return o << "Onset: " << ch.onset << "; Duration: " << ch.duration << "; Pitch: "
             << (int) ch.pitch
             << "; Velocity: " << (int) ch.velocity;
}

void printMidiEventList(const std::vector<SimpleMidiEvent> &evtlist) {
    std::cout << std::setw(9) << "Onset " << std::setw(9) << "Duration " << std::setw(9) << "Pitch " << std::setw(9)
              << "Velocity " << std::endl;
    for (auto it : evtlist) {
        std::cout << std::setw(8) << it.onset << " " << std::setw(8) << it.duration << " " << std::setw(8)
                  << (int) it.pitch << " " << std::setw(8)
                  << (int) it.velocity << std::endl;
    }
}

SimpleMidiEventList convertMidiFile(const smf::MidiFile& midifile) {
    std::vector<SimpleMidiEvent> evtlist;
    int32_t first_onset = -1;
    int tracks = midifile.getTrackCount();
    for (int track = 0; track < tracks; track++) {
        for (int event = 0; event < midifile[track].size(); event++) {
            if (midifile[track][event].isNoteOn()) {
                if (first_onset == -1)
                    first_onset = midifile[track][event].tick;
                evtlist.emplace_back(fromMidiEvent(first_onset, midifile[track][event]));
            }
        }
    }
    return {evtlist};
}

std::vector<SimpleMidiEvent> convertMidiEvents(const smf::MidiEventList &evtList) {
    std::vector<SimpleMidiEvent> evts;
    int start = 0;
    for (size_t i = 0; i < evtList.size(); ++i) {
        auto event = evtList[i];
        if (event.isNoteOn()) {
            if (start < 0) start = event.tick;
            evts.emplace_back(fromMidiEvent(start, event));
        }
    }
    return evts;
}

MidiString loadMidiString(const std::string &path) {
    smf::MidiFile midifile;
    midifile.read(path);
    midifile.doTimeAnalysis();
    midifile.linkNotePairs();

    std::vector<SimpleMidiEvent> evtlist;
    int tracks = midifile.getTrackCount();
    int start = -1;
    for (int track = 0; track < tracks; track++) {
        for (int event = 0; event < midifile[track].size(); event++) {
            if (midifile[track][event].isNoteOn()) {
                if (start == -1)
                    start = midifile[track][event].tick;
                evtlist.emplace_back(fromMidiEvent(start, midifile[track][event]));
            }
        }
    }
    std::cout << "Loading MIDI file: " << path << std::endl;
    std::cout << "Tick Rate: " << midifile.getTicksPerQuarterNote() << "/quarter note" << std::endl;
    // printMidiEventList(evtlist);
    return evtlist;
}

const char *degToCol(size_t r) {
    if (r == 0) {
        return "red";
    } else if (r > 1) {
        return "yellow";
    }
    return "white";
}

void PrintGraphViz(const WeightedBipartiteGraph<SimpleMidiEvent> &g, std::ostream &o) {
    o << "graph \"name\" {" << std::endl;
    // Credit: https://stackoverflow.com/a/44274606
    // Show left/right on the same rank, and preserve ordering of the two sets
    o << "rank1 [style=invisible];\nrank2 [style=invisible];" << std::endl;
    o << "rank1 -- rank2 [color=white];" << std::endl;

    // Node labels / properties
    for (size_t i = 0; i < g.GetL().size(); ++i) {
        o << "l" << i << " [label=\"" << pitchToNote(g.GetL()[i].pitch) << "\";style=filled;fillcolor="
          << degToCol(g.GetLNodeDegree(i)) << "]" << std::endl;
    }
    for (size_t i = 0; i < g.GetR().size(); ++i) {
        o << "r" << i << " [label=\"" << pitchToNote(g.GetR()[i].pitch) << "\";style=filled;fillcolor="
          << degToCol(g.GetRNodeDegree(i)) << "]" << std::endl;
    }

    // Edges between l/r
    for (size_t i = 0; i < g.GetL().size(); ++i) {
        for (auto e : g.GetLNodeEdges(i)) {
            o << "l" << i << " -- " << "r" << e.to;
            if (g.GetL()[i].pitch != g.GetR()[e.to].pitch) {
                o << "[color=red;penwidth=5]";
            }
            o << std::endl;
        }
    }

    // Ordering within L
    o << "{rank=same;rank1 " << std::endl;
    for (size_t i = 0; i < g.GetL().size(); ++i) {
        o << "-- l" << i;
    }
    o << "[style=invis];rankdir=LR}" << std::endl;

    // Ordering within R
    o << "{rank=same;rank2 " << std::endl;
    for (size_t i = 0; i < g.GetR().size(); ++i) {
        o << "-- r" << i;
    }
    o << "[style=invis];rankdir=LR}" << std::endl;
    o << "}" << std::endl;
}
