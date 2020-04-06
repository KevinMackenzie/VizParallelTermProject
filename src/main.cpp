#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace smf;

// Encodes "notes" as they are played and released
struct SimpleMidiEvent {
  uint32_t onset;
  uint16_t duration;
  uint8_t pitch;
  uint8_t velocity;

  SimpleMidiEvent() : onset(0), duration(0), pitch(0), velocity(0) {}
};

SimpleMidiEvent fromMidiEvent(const MidiEvent& mevt) {
  SimpleMidiEvent evt;
  evt.duration = mevt.getTickDuration();
  evt.onset = mevt.tick;
  evt.pitch = mevt.getKeyNumber();
  evt.velocity = mevt.getVelocity();
  return evt;
}

// A single "char" in the "string" used in edit-distance
struct MidiChar {
  // The current state of the notes last played on the keyboard
  //  NOTE: Attenuation is not factored in yet, and the pedal is not considered
  SimpleMidiEvent keyboardContext[128];
  // The midi event corresponding to this "char"
  SimpleMidiEvent event;
};

using MidiString = std::vector<MidiChar>;

MidiString constructMidiString(std::vector<SimpleMidiEvent> evts) {
  MidiString ret;
  ret.reserve(evts.size());

  MidiChar curr;
  for (auto evt : evts) {
    for (auto& i : curr.keyboardContext) {
      if (i.duration + i.onset < evt.onset) {
        i = SimpleMidiEvent();
      }
    }
    curr.event = evt;
    ret.emplace_back(curr);
  }
  return ret;
}

MidiString loadMidiString(const std::string& path) {
  MidiFile midifile;
  midifile.read(path);
  midifile.doTimeAnalysis();
  midifile.linkNotePairs();

  std::vector<SimpleMidiEvent> evtlist;
  int tracks = midifile.getTrackCount();
  for (int track = 0; track < tracks; track++) {
    for (int event = 0; event < midifile[track].size(); event++) {
      if (midifile[track][event].isNoteOn()) {
        evtlist.emplace_back(fromMidiEvent(midifile[track][event]));
      }
    }
  }
  return constructMidiString(evtlist);
}

struct MemoVal {
  size_t weight;
};

void editDistance(const MidiString& ref, const MidiString& inp) {
  if (ref.empty() || inp.empty()) return;
  std::vector<std::vector<MemoVal> > memo(ref.size() + 1, std::vector<MemoVal>(inp.size() + 1, MemoVal()));

  // Fill the base-case row / column in the memo
  for (size_t i = 0; i < ref.size() + 1; ++i) {
    memo[i][0].weight = i;
  }
  for (size_t i = 0; i < inp.size() + 1; ++i) {
    memo[0][i].weight = i;
  }

  size_t ref_idx = 1;
  for (auto ref_val : ref) {
    size_t inp_idx = 1;
    for (auto inp_val : inp) {
      // Insertion
      auto ref_m1 = memo[ref_idx - 1][inp_idx];
      ref_m1.weight += 1;

      // Deletion
      auto inp_m1 = memo[ref_idx][inp_idx - 1];
      inp_m1.weight += 1;

      // Substitution (or match)
      auto prev = memo[ref_idx - 1][inp_idx - 1];
      prev.weight += ref_val.event.pitch == inp_val.event.pitch ? 0 : 1;

      // cout << "Comparing " << (int) ref_val.event.pitch << " and " << (int) inp_val.event.pitch;
      if (prev.weight < ref_m1.weight && prev.weight < inp_m1.weight) {
        // cout << "; Chose Substitution / Match";
        memo[ref_idx][inp_idx] = prev;
      } else if (ref_m1.weight < inp_m1.weight) {
        // cout << "; Chose Insertion";
        memo[ref_idx][inp_idx] = ref_m1;
      } else {
        // cout << "; Chose Deletion";
        memo[ref_idx][inp_idx] = inp_m1;
      }
      // cout << endl;

      ++inp_idx;
    }
    ++ref_idx;
  }

  cout << "Edit Distance Result: " << memo.back().back().weight << endl;
}

void printMidiEventList(const std::vector<SimpleMidiEvent>& evtlist) {
  cout << setw(9) << "Onset " << setw(9) << "Duration " << setw(9) << "Pitch " << setw(9) << "Velocity " << endl;
  for (auto it : evtlist) {
    cout << setw(8) << it.onset << " " << setw(8) << it.duration << " " << setw(8) << (int) it.pitch << " " << setw(8)
         << (int) it.velocity << endl;
  }
}

int main(int argc, char** argv) {
  Options options;
  options.process(argc, argv);

  MidiString ref = loadMidiString(options.getArg(1));
  MidiString inp = loadMidiString(options.getArg(2));

  editDistance(ref, inp);

  return 0;
}