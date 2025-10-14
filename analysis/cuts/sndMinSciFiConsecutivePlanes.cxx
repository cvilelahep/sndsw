#include "sndMinSciFiConsecutivePlanes.h"

#include "sndSciFiTools.h"

#include "TChain.h"

namespace snd::analysis_cuts{
  minSciFiConsecutivePlanes::minSciFiConsecutivePlanes() : sciFiBaseCut(){
    processName = "Two or more consecutive SciFi planes";

    shortName = "At least two consecutive SciFi planes";
    nbins = std::vector<int>{1};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{1};
    plot_var = std::vector<double>{-1};

  }

  void minSciFiConsecutivePlanes::process(){
    initializeEvent();
    
    // For a plane to count, need both planes to have hits
    for (long unsigned int i = 0; i < hits_per_plane_horizontal->size() - 1; i++){
      if (hits_per_plane_horizontal->at(i) * hits_per_plane_vertical->at(i)
	  *hits_per_plane_horizontal->at(i+1) * hits_per_plane_vertical->at(i+1) > 0) {passed_cut = true; return;}
    }
    passed_cut = false; return;
  }
}	     
