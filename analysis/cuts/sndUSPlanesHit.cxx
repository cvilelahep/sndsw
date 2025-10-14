#include "sndUSPlanesHit.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"
#include "TString.h"

#include <vector>
#include <map>
#include <numeric>

namespace snd {
  namespace analysis_cuts {
  
    USPlanesHit::USPlanesHit(std::vector<int> planes_hit) : MuFilterBaseCut(), planes_hit_(planes_hit) {

      processName = "Planes hit ";
      for (int plane : planes_hit_) processName += " "+std::to_string(plane);
      
      shortName = "USPlanesHit";
      nbins = std::vector<int>{1};
      range_start = std::vector<double>{0};
      range_end = std::vector<double>{1};
      plot_var = std::vector<double>{-1};
    }
    
    void USPlanesHit::process(){

      std::vector<bool> us = std::vector<bool>(planes_hit_.size(), false);

      for (TObject * obj : *muFilterDigiHitCollection){
	MuFilterHit * hit = dynamic_cast<MuFilterHit*>(obj);

	if (! hit->isValid()) continue;

	if (hit->GetSystem() == 2) {
	  for (unsigned long int i_plane = 0; i_plane < planes_hit_.size(); i_plane++) {
	    if (hit->GetPlane() == planes_hit_[i_plane]) us[i_plane] = true;
	  }
	}
      }
      
      for (bool this_plane_is_hit: us){
	if (not this_plane_is_hit){
	  passed_cut = false; return;
	}
      }
      passed_cut = true; return;
    }

  }
}
