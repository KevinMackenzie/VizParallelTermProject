#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include "Analysis.h"

using namespace smf;

int main(int argc, char** argv) {
  Options options;
  options.process(argc, argv);

  MidiString ref = loadMidiString(options.getArg(1));
  MidiString inp = loadMidiString(options.getArg(2));

  clock_t start = clock();
  auto g = editDistancePDiagnal(ref, inp);
  clock_t end = clock();

  std::cout << "Edit Distance Result: " << g.GetTotalWeight() << std::endl;
  // std::cout << g;

  std::cout << "Took " << (double)(end-start)/CLOCKS_PER_SEC << " Seconds" << std::endl;

  std::ofstream ofile("graph.dot");
  PrintGraphViz(g, ofile);
  ofile.close();

  return 0;
}