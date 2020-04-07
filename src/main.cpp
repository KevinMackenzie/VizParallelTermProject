#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>
#include <cmath>

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

std::string pitchToNote(uint8_t pitch) {
  char pitches[][12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
  std::stringstream ss;
  ss << pitches[pitch % 12] << -1 + pitch/12;
  return ss.str();
}
std::ostream& operator<<(std::ostream& o, const MidiChar& ch) {
  return o << "Onset: " << ch.event.onset << "; Duration: " << ch.event.duration << "; Pitch: " << (int) ch.event.pitch
           << "; Velocity: " << (int) ch.event.velocity;
}

using MidiString = std::vector<MidiChar>;

void printMidiEventList(const std::vector<SimpleMidiEvent>& evtlist) {
  cout << setw(9) << "Onset " << setw(9) << "Duration " << setw(9) << "Pitch " << setw(9) << "Velocity " << endl;
  for (auto it : evtlist) {
    cout << setw(8) << it.onset << " " << setw(8) << it.duration << " " << setw(8) << (int) it.pitch << " " << setw(8)
         << (int) it.velocity << endl;
  }
}

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
  cout << "Loading MIDI file: " << path << endl;
  printMidiEventList(evtlist);
  return constructMidiString(evtlist);
}

// Adjacency list implementation of a bipartite graph
template<typename T>
class WeightedBipartiteGraph {
public:
  struct edge_type {
    size_t to;
    float weight;
  };
private:
  // Note: This has very poor locality.  If its an issue,
  //  we can use an in-place vector and move to the heap
  //  only for abnormally large adjacency lists per node
  std::vector<std::vector<edge_type>> ltr;
  std::vector<std::vector<edge_type>> rtl;
  const std::vector<T>* left;
  const std::vector<T>* right;
  float total_weight;

  static typename std::vector<edge_type>::iterator tfind(std::vector<edge_type>& vec, size_t t) {
    auto it = vec.begin();
    while (it != vec.end() && it->to != t) ++it;
    return it;
  }
  static const char* degToCol(size_t r) {
    if (r == 0) {
      return "red";
    } else if (r > 1) {
      return "yellow";
    }
    return "white";
  }

public:
  WeightedBipartiteGraph(const std::vector<T>& l, const std::vector<T>& r) : left(&l), right(&r), ltr(l.size()),
                                                                             rtl(r.size()), total_weight(0) {}

  WeightedBipartiteGraph(const WeightedBipartiteGraph<T>& other) : ltr(other.ltr), rtl(other.rtl),
                                                                   left(other.left), right(other.right),
                                                                   total_weight(other.total_weight) {}

  WeightedBipartiteGraph<T>& operator=(const WeightedBipartiteGraph<T>& other) {
    ltr = other.ltr;
    rtl = other.rtl;
    left = other.left;
    right = other.right;
    total_weight = other.total_weight;
    return *this;
  }

  void AddEdge(size_t l, size_t r, float w) {
    auto it = tfind(ltr[l], r); // TODO: segfaulting here
    if (it != ltr[l].end())  // Prevent duplicates (not multigraph)
      return;
    ltr[l].push_back({r, w});
    rtl[r].push_back({l, w});
    total_weight += w;
  }

  float GetEdgeWeight(size_t l, size_t r) const {
    auto it = std::find(ltr[l], r);
    if (it != ltr[l].end()) {
      return it->weight;
    }
    return INFINITY;
  }

  float GetTotalWeight() const { return total_weight; }

  float GetLNodeDegree(size_t l) const { return ltr[l].size(); }

  float GetRNodeDegree(size_t r) const { return rtl[r].size(); }

  const std::vector<edge_type>& GetLNodeEdges(size_t l) const { return ltr[l]; }

  const std::vector<edge_type>& GetRNodeEdges(size_t r) const { return rtl[r]; }

  void RemoveEdge(size_t l, size_t r) {
    // Although this is linear wrt the size of the vectors,
    //  we expect that adjacency will be small
    auto it0 = tfind(ltr[l], r);
    if (it0 != ltr[l].end()) {
      total_weight -= it0->weight;
      ltr[l].erase(it0);
    }
    auto it1 = tfind(rtl[r], l);
    if (it1 != rtl[r].end()) {
      rtl[r].erase(it1);
    }
  }

  friend std::ostream& operator<<(std::ostream& o, const WeightedBipartiteGraph<T>& g) {
    o << "Weighted Graph:" << std::endl;
    o << "Left Nodes" << std::endl;
    for (size_t i = 0; i < g.left->size(); ++i) {
      o << "Node " << i << " (" << (*g.left)[i] << ")" << std::endl;
      for (size_t j = 0; j < g.ltr[i].size(); ++j) {
        o << "  " << g.ltr[i][j].to << ": " << g.ltr[i][j].weight << std::endl;
      }
    }
    o << "Right Nodes" << std::endl;
    for (size_t i = 0; i < g.right->size(); ++i) {
      o << "Node " << i << " (" << (*g.right)[i] << ")" << std::endl;
      for (size_t j = 0; j < g.rtl[i].size(); ++j) {
        o << "  " << g.rtl[i][j].to << ": " << g.rtl[i][j].weight << std::endl;
      }
    }
    return o;
  }

  void PrintGraphViz(std::ostream& o) {
    o << "graph \"name\" {" << std::endl;
    // Credit: https://stackoverflow.com/a/44274606
    // Show left/right on the same rank, and preserve ordering of the two sets
    o << "rank1 [style=invisible];\nrank2 [style=invisible];" << std::endl;
    o << "rank1 -- rank2 [color=white];" << std::endl;

    // Node labels / properties
    for (size_t i = 0; i < left->size(); ++i) {
      o << "l" << i << " [label=" << pitchToNote((*left)[i].event.pitch) << ";style=filled;fillcolor=" << degToCol(ltr[i].size()) << "]" << std::endl;
    }
    for (size_t i = 0; i < right->size(); ++i) {
      o << "r" << i << " [label=" << pitchToNote((*right)[i].event.pitch) << ";style=filled;fillcolor=" << degToCol(rtl[i].size()) << "]" << std::endl;
    }

    // Edges between l/r
    for (size_t i = 0; i < ltr.size(); ++i) {
      for (size_t j = 0; j < ltr[i].size(); ++j) {
        o << "l" << i << " -- " << "r" << ltr[i][j].to << std::endl;
      }
    }

    // Ordering within L
    o << "{rank=same;rank1 " << std::endl;
    for (size_t i = 0; i < left->size(); ++i) {
      o << "-- l" << i;
    }
    o << "[style=invis];rankdir=LR}" << std::endl;

    // Ordering within R
    o << "{rank=same;rank2 " << std::endl;
    for (size_t i = 0; i < right->size(); ++i) {
      o << "-- r" << i;
    }
    o << "[style=invis];rankdir=LR}" << std::endl;
    o << "}" << std::endl;
  }
};

struct MemoVal {
  WeightedBipartiteGraph<MidiChar> g;
};

float weight_func(const MidiChar& ch0, const MidiChar& ch1) {
  float pitch_comp = abs(ch0.event.pitch - ch1.event.pitch);
  float time_comp = abs((float) ch0.event.onset - ch1.event.onset) / 1000.f;
  return pitch_comp + time_comp;
}

void addMinWeightInsert(MemoVal& m, size_t curr_inp_idx, const MidiChar& inp_val, const std::vector<MidiChar>& ref, size_t curr_ref_idx) {
  // Handle the base case
  if (curr_ref_idx == 0)
    return;

  // Find the minimum edge weight
  size_t min_idx = 0;
  float min_weight = INFINITY;
  for (size_t i = 0; i < curr_ref_idx; ++i) { // Note: We don't look at the "CURRENT" ref index, only up to
    float w = weight_func(ref[i], inp_val);
    if (w < min_weight) {
      min_idx = i;
      min_weight = w;
    }
  }
  // Add it to the graph
  m.g.AddEdge(min_idx, curr_inp_idx, min_weight);

  // Find all higher cost edges that don't orphan nodes if removed
  std::vector<size_t> to_remove;
  for (auto it : m.g.GetLNodeEdges(min_idx)) {
    if (it.weight > min_weight && m.g.GetRNodeDegree(it.to) > 1) {
      to_remove.emplace_back(it.to);
    }
  }
  // Remove them
  for (auto it : to_remove) {
    m.g.RemoveEdge(min_idx, it);
  }
}

void addMinWeightDelete(MemoVal& m, size_t curr_ref_idx, const MidiChar& ref_val, const std::vector<MidiChar>& inp, size_t curr_inp_idx) {
  // Handle the base case
  if (curr_inp_idx == 0)
    return;

  // Find the minimum edge weight
  size_t min_idx = 0;
  float min_weight = INFINITY;
  for (size_t i = 0; i < curr_inp_idx; ++i) { // Note: only looks at 'prev' and not 'curr'
    float w = weight_func(ref_val, inp[i]);
    if (w < min_weight) {
      min_idx = i;
      min_weight = w;
    }
  }
  // Add it to the graph
  m.g.AddEdge(curr_ref_idx, min_idx, min_weight);

  // Find all higher cost edges that don't orphan nodes if removed
  std::vector<size_t> to_remove;
  for (auto it : m.g.GetRNodeEdges(min_idx)) {
    if (it.weight > min_weight && m.g.GetLNodeDegree(it.to) > 1) {
      to_remove.emplace_back(it.to);
    }
  }
  // Remove them
  for (auto it : to_remove) {
    m.g.RemoveEdge(it, min_idx);
  }
}

void editDistance(const MidiString& ref, const MidiString& inp) {
  if (ref.empty() || inp.empty()) return;
  std::vector<std::vector<MemoVal> > memo(ref.size() + 1,
                                          std::vector<MemoVal>(inp.size() + 1,
                                                               {WeightedBipartiteGraph<MidiChar>(ref, inp)}));

  // Fill the base-case row / column in the memo
  // for (size_t i = 0; i < ref.size() + 1; ++i) {
  //   memo[i][0].g =;
  // }
  // for (size_t i = 0; i < inp.size() + 1; ++i) {
  //   memo[0][i].g =
  // }

  size_t ref_idx = 1;
  for (auto ref_val : ref) {
    size_t inp_idx = 1;
    for (auto inp_val : inp) {
      // Insertion: Connect new input to existing ref
      MemoVal ref_m1 = memo[ref_idx - 1][inp_idx];
      addMinWeightInsert(ref_m1, inp_idx - 1, inp_val, ref, ref_idx - 1);

      // Deletion: Connect new ref to existing input
      MemoVal inp_m1 = memo[ref_idx][inp_idx - 1];
      addMinWeightDelete(inp_m1, ref_idx - 1, ref_val, inp, inp_idx - 1);

      // Substitution (or match)
      MemoVal prev = memo[ref_idx - 1][inp_idx - 1];
      addMinWeightInsert(prev, inp_idx - 1, inp_val, ref, ref_idx);
      addMinWeightDelete(prev, ref_idx - 1, ref_val, inp, inp_idx);

      // TODO: how do we adjust the weight for the insertion / deletion case?  Or do we event want to?
      float ins_weight = ref_m1.g.GetTotalWeight();
      float del_weight = inp_m1.g.GetTotalWeight();
      float mat_weight = prev.g.GetTotalWeight();

      cout << "Comparing " << (int) ref_val.event.pitch << " and " << (int) inp_val.event.pitch;
      if (ins_weight < del_weight && ins_weight < mat_weight) {
        cout << "; Chose Insertion";
        memo[ref_idx][inp_idx] = ref_m1;
      } else if (del_weight < mat_weight) {
        cout << "; Chose Deletion";
        memo[ref_idx][inp_idx] = inp_m1;
      } else {
        cout << "; Chose Substitution / Match";
        memo[ref_idx][inp_idx] = prev;
      }
      cout << endl;

      ++inp_idx;
    }
    ++ref_idx;
  }

  std::cout << "Edit Distance Result: " << memo.back().back().g.GetTotalWeight() << endl;
  std::cout << memo.back().back().g;

  std::ofstream ofile("graph.dot");
  memo.back().back().g.PrintGraphViz(ofile);
  ofile.close();
}

int main(int argc, char** argv) {
  Options options;
  options.process(argc, argv);

  MidiString ref = loadMidiString(options.getArg(1));
  MidiString inp = loadMidiString(options.getArg(2));

  editDistance(ref, inp);

  return 0;
}