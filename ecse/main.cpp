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

struct pt {
    size_t r, i;
};
void testDiagonal() {
    size_t r = 4, i=4;
    std::vector<pt> idxs;
    std::vector<std::vector<int> > pixs(r+1, std::vector<int>(i+1, 0));
    for (size_t d_idx = 1; d_idx <= r + i; ++d_idx) {
        // Generate vector of indexes to process in this diagonal
        idxs.clear();
        idxs.reserve(d_idx);
        pt p;
        p.r = d_idx;
        p.i = 1;
        while (p.r > r) {
            p.r--;
            p.i++;
        }
        while (p.i <= i && p.r > 0) {
            idxs.push_back(p);
            p.r--;
            p.i++;
        }

        for (auto idx : idxs) {
            pixs[idx.r][idx.i] = d_idx;
        }
    }
    std::cout << std::endl;
    std::cout << "+";
    for (auto c : pixs[0]) {
        std::cout << "--+";
    }
    std::cout << std::endl;
    for (auto r : pixs) {
        std::cout << "|";
        for (auto c : r) {
            std::cout << std::setw(2) << c << "|";
        }
        std::cout << std::endl;
        std::cout << "+";
        for (auto c : r) {
            std::cout << "--+";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char **argv) {
    Options options;
    options.process(argc, argv);

    MidiString ref = loadMidiString(options.getArg(1));
    MidiString inp = loadMidiString(options.getArg(2));
    size_t mx = argc < 4 ? 0 : atoi(argv[3]);

    // std::cout << "Reference:" << std::endl;
    // filterTempo(ref, 0);
    // for (auto ch : ref) {
    //     std::cout << "Pitch: " << ch.event.pitch << "; Tempo: " << ch.tempo << ";" << std::endl;
    // }
    // filterTempo(inp, 0);
    // std::cout << "Input:" << std::endl;
    // for (auto ch : inp) {
    //     std::cout << "Pitch: " << ch.event.pitch << "; Tempo: " << ch.tempo << ";" << std::endl;
    // }

    std::cout << "Ref Size: " << ref.size() << std::endl;
    std::cout << "Inp Size: " << inp.size() << std::endl;

    if (mx) {
        ref.resize(mx);
        inp.resize(mx);
    }

    clock_t start = clock();
    auto g = editDistanceDiagonal(ref, inp, true);
    clock_t end = clock();

    cutVestigialEdges(g);

    // auto averageOnsetInt = (float)(ref.back().event.onset - ref[0].event.onset) / ref.size();
    // std::cout << "Average OI " << averageOnsetInt << std::endl;

    // auto lt = filterOnsetFrequency(ref, 1000);
    // std::ofstream o1("tempoL.csv");
    // for (size_t i = 0; i < lt.size(); ++i) {
    //     o1 << ref[i].event.onset << "," << lt[i] << std::endl;
    // }
    // o1.close();

    // auto rt = filterOnsetFrequency(inp, 1000);
    // std::ofstream o2("tempoR.csv");
    // for (size_t i = 0; i < rt.size(); ++i) {
    //     o2 << inp[i].event.onset << "," << rt[i] << std::endl;
    // }
    // o2.close();

    // auto ts = filterTimeStretch(g, lt, rt, 100);
    // std::ofstream o3("stretch.csv");
    // for (size_t i = 0; i < ts.size(); ++i) {
    //     o3 << inp[i].event.onset << "," << ts[i] << std::endl;
    // }
    // o3.close();

    // std::cout << "Edit Distance Result: " << g.GetTotalWeight() << std::endl;
    // std::cout << g;

    // std::cout << "Took " << (double) (end - start) / CLOCKS_PER_SEC << " Seconds" << std::endl;

    // std::cout << "Weights: " << std::endl;
    // for (size_t i = 0; i < ref.size(); ++i) {
    //     std::cout << "Ref Node: " << pitchToNote(ref[i].event.pitch) << "; (" << ref[i].event.onset << ");" << std::endl;
    //     for (auto a : g.GetLNodeEdges(i)) {
    //         std::cout << "to " << pitchToNote(inp[a.to].event.pitch) << " (" << inp[a.to].event.onset << ")";
    //         weight_func(ref[i], inp[a.to]);
    //     }
    //     std::cout << std::endl;
    // }


    std::ofstream ofile("graph.dot");
    PrintGraphViz(g, ofile);
    ofile.close();
    std::ofstream ofile1("graph.txt");
    PrintGraph(g, ofile1);
    ofile1.close();

    return 0;
}