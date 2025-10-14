#include "sndStableBeams.h"

#include "SNDLHCEventHeaderConst.h"

#include <iostream>

namespace snd::analysis_cuts {

  stableBeamsCut::stableBeamsCut() : EventHeaderBaseCut() {
    processName = "Stable beams";

    shortName = "StableBeams";
    nbins = std::vector<int>{31};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{31};
    plot_var = std::vector<double>{-1};

  }

  void stableBeamsCut::process(){

    plot_var[0] = header->GetBeamMode();
    
    if (header->GetBeamMode() == static_cast<int>(LhcBeamMode::StableBeams)) {passed_cut = true; return;}
    else {passed_cut = false; return;}
  }
}
