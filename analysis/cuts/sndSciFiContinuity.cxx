#include "sndSciFiContinuity.h"

#include "sndSciFiTools.h"

#include "TChain.h"

namespace snd {
  namespace analysis_cuts {

    sciFiContinuity::sciFiContinuity() : sciFiBaseCut(){
      processName = "SciFi continuity";

      shortName = "SciFiContinuity";
      nbins = std::vector<int>{1};
      range_start = std::vector<double>{0};
      range_end = std::vector<double>{1};
      plot_var = std::vector<double>{-1};

    }

    void sciFiContinuity::process(){
      initializeEvent();
    
      bool gotFirstPlane = false;
      // For a plane to count, need both planes to have hits
      for (unsigned long int i_plane = 0; i_plane < hits_per_plane_horizontal->size(); i_plane++){
	if ((hits_per_plane_horizontal->at(i_plane)*hits_per_plane_vertical->at(i_plane) == 0) and gotFirstPlane){
	  passed_cut = false; return;
	} else if (hits_per_plane_horizontal->at(i_plane)*hits_per_plane_vertical->at(i_plane) > 0){
	  gotFirstPlane = true;
	}
      }
      passed_cut = true; return;
    }
  }	     
}
