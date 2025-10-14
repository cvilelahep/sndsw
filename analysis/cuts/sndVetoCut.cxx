#include "sndVetoCut.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"

namespace snd::analysis_cuts {

  vetoCut::vetoCut() : MuFilterBaseCut() {
    processName = "No hits in veto";

    shortName = "NoVetoHits";
    nbins = std::vector<int>{16};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{16};
    plot_var = std::vector<double>{-1};

  }

  void vetoCut::process(){
    plot_var[0] = 0;

    for (TObject * obj : *muFilterDigiHitCollection){
      MuFilterHit * hit = dynamic_cast<MuFilterHit*>(obj);
      if (hit->GetSystem() == 1) plot_var[0] += 1;
    }
    
    if (plot_var[0] > 0) {passed_cut = false; return;}
    passed_cut = true; return;
  }
}
