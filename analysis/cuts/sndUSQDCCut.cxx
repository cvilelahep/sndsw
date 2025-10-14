#include "sndUSQDCCut.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"
#include "TString.h"

#include <vector>
#include <map>
#include <numeric>

namespace snd::analysis_cuts {

  USQDCCut::USQDCCut(float qdc_threshold) : MuFilterBaseCut(), qdc_threshold_(qdc_threshold) {
    processName = "Total US QDC > "+std::to_string(qdc_threshold_);

    shortName = "USQDC";
    nbins = std::vector<int>{100};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{10000};
    plot_var = std::vector<double>{-1};
  }

  void USQDCCut::process(){

    float totQDC = 0.;
    
    std::vector<bool> us = std::vector<bool>(5, false); 
    
    for (TObject * obj : *muFilterDigiHitCollection){
      MuFilterHit * hit = dynamic_cast<MuFilterHit*>(obj);
      
      if (!hit->isValid()) continue;
      if (hit->GetSystem() == 2) {
	// TO DO: Implement flag to skip small SiPMs
	for (const auto& [key, value] : hit->GetAllSignals()) {
	  totQDC += value;
	}
      }
    }
    plot_var[0] = totQDC;
    if (totQDC >= qdc_threshold_) {passed_cut = true; return;}
    passed_cut = false; return;
  }
}
