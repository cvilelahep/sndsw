#include "sndSciFiStationCut.h"

#include "sndSciFiTools.h"

#include "TChain.h"

namespace snd::analysis_cuts{
  sciFiStationCut::sciFiStationCut(float fraction_threshold, std::vector<int> excluded_stations) : sciFiBaseCut(), fraction_threshold_(fraction_threshold), excluded_stations_(excluded_stations){

    processName = "Exclude stations";
    for (int sta : excluded_stations_){
      processName += " "+std::to_string(sta);
    }
    processName += ". Threshold "+std::to_string(fraction_threshold_);

    shortName = "SciFiStation";
    for (int sta : excluded_stations_) shortName += "_"+std::to_string(sta);

    nbins = std::vector<int>{5};
    range_start = std::vector<double>{1};
    range_end = std::vector<double>{6};
    plot_var = std::vector<double>{-1};
  }

  void sciFiStationCut::process(){
    initializeEvent();
    
    int station = snd::analysis_tools::findScifiStation(*hits_per_plane_horizontal, *hits_per_plane_vertical, fraction_threshold_);
    
    plot_var[0] = station;

    if (std::find(excluded_stations_.begin(), excluded_stations_.end(), station) == excluded_stations_.end()){
      passed_cut = true; return;
    } else {
      passed_cut = false; return;
    }
  }
}	     
