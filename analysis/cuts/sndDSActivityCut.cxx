#include "sndDSActivityCut.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"

#include <vector>
#include <numeric>

namespace snd::analysis_cuts {

  DSActivityCut::DSActivityCut() : MuFilterBaseCut() {
    processName = "If there are DS hits, all US planes must be hit";
    
    shortName = "NUSPlanesHitIfDSHit";
    nbins = std::vector<int>{5};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{5};
    plot_var = std::vector<double>{-1};

  }

  void DSActivityCut::process(){
    MuFilterHit * hit;
    TIter hitIterator(muFilterDigiHitCollection);

    bool ds = false;
    std::vector<bool> us = std::vector<bool>(5, false); 

    while ( (hit = (MuFilterHit*) hitIterator.Next()) ){
      if (hit->GetSystem() == 2) {
	us[hit->GetPlane()] = true;
      } else if (hit->GetSystem() == 3) {
	ds = true;
      }
    }
    
    if (not ds) {
      plot_var[0] = -1;
      passed_cut = false; return;
    }

    plot_var[0] = std::accumulate(us.begin(), us.end(), 0);
    if (plot_var[0] == 5) {passed_cut = true; return;}
    passed_cut = false; return;
  }
}
