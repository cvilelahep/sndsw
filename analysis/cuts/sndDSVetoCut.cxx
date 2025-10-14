#include "sndDSVetoCut.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"

#include <vector>
#include <numeric>

namespace snd::analysis_cuts {

  DSVetoCut::DSVetoCut() : MuFilterBaseCut() {
    processName = "Remove events with hits in the last (hor) and two last (ver) DS planes";
    
    shortName = "DSVetoCut";
    nbins = std::vector<int>{180};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{180};
    plot_var = std::vector<double>{-1};

  }

  void DSVetoCut::process(){
    
    double n_hits = 0;
    
    passed_cut = true;

    for (TObject * obj : *muFilterDigiHitCollection){
      MuFilterHit * hit = dynamic_cast<MuFilterHit*>(obj);

      if (! hit->isValid()) continue;
      
      if (hit->GetSystem() == 3) { // DS
	if (hit->GetPlane() >= 2) {
	  passed_cut = false; 
	  n_hits+=1;
	}
      }
    }
    
    plot_var[0] = n_hits;
  }
}
