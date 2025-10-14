#include "sndMinSciFiHitsCut.h"

#include "sndSciFiTools.h"

#include "TChain.h"

namespace snd::analysis_cuts{
  minSciFiHits::minSciFiHits(int hit_threshold) : sciFiBaseCut(), hit_threshold_(hit_threshold){

    processName = "More than "+std::to_string(hit_threshold_)+" SciFi hits";
    shortName = "SciFiMinHits";
    nbins = std::vector<int>{1536};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{1536};
    plot_var = std::vector<double>{-1};
  }

  void minSciFiHits::process(){
    initializeEvent();
    plot_var[0] = snd::analysis_tools::getTotalSciFiHits(*hits_per_plane_horizontal, *hits_per_plane_vertical);
    if ( plot_var[0] < hit_threshold_) {passed_cut = false; return;}
    passed_cut = true; return;
  }
}	     
