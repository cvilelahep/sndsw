#include "sndUSBarsVeto.h"

#include "TClonesArray.h"
#include "TChain.h"
#include "MuFilterHit.h"
#include "TString.h"

#include <vector>
#include <map>
#include <numeric>

namespace sndAnalysis {

  USBarsVeto::USBarsVeto(std::vector<std::pair<int, int> > bars_to_exclude, TChain * tree) : MuFilterBaseCut(tree) {
    bars = bars_to_exclude;
    cutName = "Bars to exclude ";
    for (std::pair<int, int> bar : bars) cutName += " ("+std::to_string(std::get<0>(bar))+" "+std::to_string(std::get<1>(bar))+")";

    shortName = "USBarsVeto";
    nbins = std::vector<int>{1};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{1};
    plot_var = std::vector<double>{-1};
  }

  bool USBarsVeto::passCut(){
    MuFilterHit * hit;
    TIter hitIterator(muFilterDigiHitCollection);
    
    while ( (hit = (MuFilterHit*) hitIterator.Next()) ){
      if (! hit->isValid()) continue;
      
      if (hit->GetSystem() == 2) {
	int this_plane = hit->GetPlane();
	int this_bar = int(hit->GetDetectorID()%100);

	for (std::pair<int, int> bar : bars) {
	  if (this_plane == std::get<0>(bar) and this_bar == std::get<1>(bar)) return false;
	}
      }
    }
    return true;
  }
}
