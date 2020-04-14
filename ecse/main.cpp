#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include "Analysis.h"

using namespace smf;

int main(int argc, char **argv) {
    Options options;
    options.process(argc, argv);

    MidiString ref = loadMidiString(options.getArg(1));
    MidiString inp = loadMidiString(options.getArg(2));

    std::cout << "Reference:" << std::endl;
    filterTempo(ref);
    for (auto ch : ref) {
        std::cout << "Pitch: " << ch.event.pitch << "; Tempo: " << ch.tempo << ";" << std::endl;
    }
    filterTempo(inp);
    std::cout << "Input:" << std::endl;
    for (auto ch : inp) {
        std::cout << "Pitch: " << ch.event.pitch << "; Tempo: " << ch.tempo << ";" << std::endl;
    }

    clock_t start = clock();
    auto g = editDistancePDiagnal(ref, inp);
    clock_t end = clock();

    std::cout << "Edit Distance Result: " << g.GetTotalWeight() << std::endl;
    // std::cout << g;

    std::cout << "Took " << (double) (end - start) / CLOCKS_PER_SEC << " Seconds" << std::endl;

    std::cout << "Weights: " << std::endl;
    for (size_t i = 0; i < ref.size(); ++i) {
        std::cout << "Ref Node: " << pitchToNote(ref[i].event.pitch) << "; (" << ref[i].event.onset << ");" << std::endl;
        for (auto a : g.GetLNodeEdges(i)) {
            std::cout << "to " << pitchToNote(inp[a.to].event.pitch) << " (" << inp[a.to].event.onset << ")";
            weight_func(ref[i], inp[a.to]);
        }
    }

    std::ofstream ofile("graph.dot");
    PrintGraphViz(g, ofile);
    ofile.close();

    return 0;
}