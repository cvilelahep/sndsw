#include "sndAvgDSFiducialCut.h"

#include "TChain.h"

namespace snd::analysis_cuts {
  avgDSFiducialCut::avgDSFiducialCut(double vertical_min, double vertical_max, double horizontal_min, double horizontal_max) : MuFilterBaseCut(), vertical_min_(vertical_min), vertical_max_(vertical_max), horizontal_min_(horizontal_min), horizontal_max_(horizontal_max) {
    
    processName = "Avg DS Ver bar in ["+std::to_string(vertical_min_)+","+std::to_string(vertical_max_)+"] Hor in ["+std::to_string(horizontal_min_)+","+std::to_string(horizontal_max_)+"]";

    shortName = "AvgDSbar";
    nbins = std::vector<int>{60, 60};
    range_start = std::vector<double>{60, 0};
    range_end = std::vector<double>{120, 60};
    plot_var = std::vector<double>{-1, -1};
  }

  void avgDSFiducialCut::process(){
    
    double avg_ver = 0.;
    unsigned int n_ver = 0;
    double avg_hor = 0.;
    unsigned int n_hor = 0;

    MuFilterHit* hit;
    TIter hitIterator(muFilterDigiHitCollection);

    while ( (hit = (MuFilterHit*) hitIterator.Next()) ){
      if (hit->isValid()){
	if (hit->GetSystem() != 3) continue;

	int x = hit->GetDetectorID() % 1000;

	if (hit->isVertical()){
	  avg_ver += x;
	  n_ver++;
	} else {
	  avg_hor += x;
	  n_hor++;
	}
      }
    }

    if ((n_ver+n_hor) == 0) {
      plot_var[0] = -1;
      plot_var[1] = -1;
      passed_cut = false; return;
    }
    
    if (n_ver) {
      avg_ver /= n_ver;
      plot_var[0] = avg_ver;
    } else {
      plot_var[0] = -1;
    }

    if (n_hor) {
      avg_hor /= n_hor;
      plot_var[1] = avg_hor;
    } else {
      plot_var[1] = -1;
    }

    if (n_ver == 0) {passed_cut = false; return;}
    if (n_hor == 0) {passed_cut = false; return;}

    if (avg_hor < horizontal_min_) {passed_cut = false; return;}
    if (avg_hor > horizontal_max_) {passed_cut = false; return;}
    if (avg_ver < vertical_min_) {passed_cut = false; return;}
    if (avg_ver > vertical_max_) {passed_cut = false; return;}

    passed_cut = true; return;
  }
}	     
