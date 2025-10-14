#include "sndAvgSciFiFiducialCut.h"

#include "TChain.h"

namespace snd::analysis_cuts {
  avgSciFiFiducialCut::avgSciFiFiducialCut(double vertical_min, double vertical_max, double horizontal_min, double horizontal_max, bool reversed) : sciFiBaseCut(), vertical_min_(vertical_min), vertical_max_(vertical_max), horizontal_min_(horizontal_min), horizontal_max_(horizontal_max), reversed_(reversed) {

    processName = "Avg SciFi Ver channel in ["+std::to_string(vertical_min_)+","+std::to_string(vertical_max_)+"] Hor in ["+std::to_string(horizontal_min_)+","+std::to_string(horizontal_max_)+"]";

    shortName = "AvgSFChan";
    nbins = std::vector<int>{128*2, 128*2};
    range_start = std::vector<double>{0, 0};
    range_end = std::vector<double>{128*12, 128*12};
    plot_var = std::vector<double>{-1, -1};
  }

  void avgSciFiFiducialCut::process(){
    initializeEvent();
    
    double avg_ver = 0.;
    unsigned int n_ver = 0;
    double avg_hor = 0.;
    unsigned int n_hor = 0;

    sndScifiHit * hit;
    TIter hitIterator(scifiDigiHitCollection);

    while ( (hit = (sndScifiHit*) hitIterator.Next()) ){
      if (hit->isValid()){
	int mat = hit->GetMat();
	int sipm = hit->GetSiPM();
	int channel = hit->GetSiPMChan();

	int x = channel + sipm*128 + mat*4*128;

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

    if (not reversed_) {
      if (avg_hor < horizontal_min_) {passed_cut = false; return;}
      if (avg_hor > horizontal_max_) {passed_cut = false; return;}
      if (avg_ver < vertical_min_) {passed_cut = false; return;}
      if (avg_ver > vertical_max_) {passed_cut = false; return;}
    } else {
      if ((avg_hor > horizontal_min_) and (avg_hor < horizontal_max_)) {passed_cut = false; return;}
      if ((avg_ver > vertical_min_) and (avg_ver < vertical_max_)) {passed_cut = false; return;}
    }
    passed_cut = true; return;
  }
}	     
