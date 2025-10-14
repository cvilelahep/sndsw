#include "sndNtrackCut.h"

#include "TROOT.h"

#include "FairLogger.h"

namespace snd::analysis_cuts{
  nTrackCut::nTrackCut(int n_min, int n_max){
    n_min_ = n_min;
    n_max_ = n_max;

    processName = "";
    if (n_min_ > 0) processName += "More than "+std::to_string(n_min_)+" ";
    if (n_max_ >= 0) processName += "At most "+std::to_string(n_max_)+" ";
    processName += "tracks";

    shortName = "nTracks_"+std::to_string(n_min_)+"_"+std::to_string(n_max_);
    nbins = std::vector<int>{5};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{5};
    plot_var = std::vector<double>{-1};

    recoTracksCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndRecoTracks"));
    if (!recoTracksCollection){
      LOG(FATAL) << "nTrackCut ERROR: Could not find sndRecoTracks";
    }
  };
  
  void nTrackCut::process(){
    int n_tracks = recoTracksCollection->GetEntries();
    plot_var[0] = n_tracks;
    
    passed_cut = true;
    if (n_tracks < n_min_){
      passed_cut = false;
    } else if (n_max_ >= n_min_ and n_tracks > n_max_){
      passed_cut = false;
    }
  }
}
