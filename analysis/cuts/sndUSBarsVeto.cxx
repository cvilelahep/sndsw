#include "sndUSBarsVeto.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"
#include "TString.h"

#include <vector>
#include <map>
#include <numeric>

namespace snd{
  namespace analysis_cuts {

    USBarsVeto::USBarsVeto(std::vector<std::pair<int, double> > avg_per_plane, bool bottom) : MuFilterBaseCut(), avg_per_plane_(avg_per_plane), bottom_(bottom) {

      processName = "Bars to exclude ";
      for (std::pair<int, double> bar : avg_per_plane_) processName += " ("+std::to_string(std::get<0>(bar))+" "+std::to_string(std::get<1>(bar))+")";

      shortName = "USBarsVeto";
      for (std::pair<int, double> bar : avg_per_plane_) shortName += "_"+std::to_string(std::get<0>(bar))+"_"+std::to_string(std::get<1>(bar));
      
      nbins = std::vector<int>(avg_per_plane_.size(), 20);
      range_start = std::vector<double>(avg_per_plane_.size(), 0);
      range_end = std::vector<double>(avg_per_plane_.size(), 10);
      plot_var = std::vector<double>(avg_per_plane_.size(), 0.);
    }

    void USBarsVeto::process(){
      std::fill(plot_var.begin(), plot_var.end(), 0.);
      std::vector<int> bars_hit(avg_per_plane_.size(), 0);

      for (TObject * obj : *muFilterDigiHitCollection){
	MuFilterHit * hit = dynamic_cast<MuFilterHit*>(obj);

	if (! hit->isValid()) continue;
      
	if (hit->GetSystem() == 2) {
	  int this_plane = hit->GetPlane();
	  int this_bar = int(hit->GetDetectorID()%100);
	  
	  for (unsigned long int i_plane = 0; i_plane < avg_per_plane_.size(); i_plane++){
	    if (this_plane == std::get<0>(avg_per_plane_.at(i_plane))) {
	      bars_hit.at(i_plane)++;
	      plot_var.at(i_plane) += this_bar;
	    }
	  }
	}
      }

      passed_cut = true;
      for (unsigned long int i_plane = 0; i_plane < avg_per_plane_.size(); i_plane++){
	plot_var.at(i_plane) /= bars_hit.at(i_plane);
	if (bottom_){	
	  if (plot_var.at(i_plane) < std::get<1>(avg_per_plane_.at(i_plane))) {
	    passed_cut = false;
	  }
	} else {
	  if (plot_var.at(i_plane) >= std::get<1>(avg_per_plane_.at(i_plane))) {
	    passed_cut = false;
	  }
	}
      }
    }
  }
}
