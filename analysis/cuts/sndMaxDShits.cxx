#include "sndMaxDShits.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"

#include <vector>
#include <numeric>

namespace snd::analysis_cuts {

  maxDShits::maxDShits(float max_hits, bool avg_layer) : MuFilterBaseCut(), max_hits_(max_hits), avg_layer_(avg_layer) {
    processName = "Maximum DS hits "+std::to_string(max_hits_);
    
    shortName = "MaxDShits_"+std::to_string(max_hits_);
    nbins = std::vector<int>{250};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{250};
    plot_var = std::vector<double>{-1};

  }

  void maxDShits::process(){

    plot_var[0] = 0.;

    for (TObject * obj : *muFilterDigiHitCollection){
      MuFilterHit * hit = dynamic_cast<MuFilterHit*>(obj);
      if (!(hit->GetSystem() == 3)) continue;
      if (! hit->isValid()) continue;
      if (hit->GetPlane() == 3){
	plot_var[0] += 1; // Only one layer in last plane
      } else {
	if (avg_layer_) plot_var[0] += 0.5; // Average over two layers in plane
	else plot_var[0] += 1;
      }
    }
    passed_cut = plot_var[0] <= max_hits_;
  }
}
