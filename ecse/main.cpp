#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include "Analysis.h"

// NOTE: IMPLEMENT THIS:
//  Change the graph rep to be two arrays: one is a fixed-length that stores the first index in the second array that refers
//      to edges starting at that index.  The second array is an array of indexes of "to" nodes that _is_ variable length
//      but will be proportional to the number of nodes.  16 bits for indexing limits us to 65k notes, but make this easy
//      to swap out.  We can be conservative and allow for an average connectivity of 2 or 3, or probably up to 10
//      This allows the algorithm to be easily ported to the GPU b/c fixed dynamic memory (textures)
//      This should mean each "graph" has a linear amount of data wrt the larger of the two parts
//      If we assume that only recent additions to the graph will be removed/rearranged, only a small number of values will need to
//      be moved around in those cases.  It would be expensive if we insert at the first element
//
//  Each graph should also keep track of the number of nodes of degree zero continuously so calculating orphans is constant-time
//
//  Change the algorithm so that it keeps a linear number of graphs alive at a time.  The memo only needs to keep the
//      graphs that are depended on, so if we do the diagonal method, we can throw out all graphs that aren't in the
//      previous two diagonal lines
//
//
//  How can we optimize the algorithm so it doesn't depend on linear numbers of dependent steps?
//
//

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

    std::cout << "Ref Size: " << ref.size() << std::endl;
    std::cout << "Inp Size: " << inp.size() << std::endl;

    ref.resize(500);
    inp.resize(500);

    clock_t start = clock();
    auto g = editDistance(ref, inp);
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
        std::cout << std::endl;
    }


    std::ofstream ofile("graph.dot");
    PrintGraphViz(g, ofile);
    ofile.close();

    return 0;
}