#include "sndSciFiHitFilter.h"

#include "TROOT.h"
#include "TGlobal.h"
#include "TInterpreter.h"
#include "TH1D.h"

#include "Scifi.h"
#include "sndScifiHit.h"
#include "ShipUnit.h"

#include "sndSciFiTools.h"

#include "FairLogger.h"

namespace snd{
  namespace analysis_processes{
    sciFiHitFilter::sciFiHitFilter(float time_lower_range, float time_upper_range, float bins_x, float min_x, float max_x){

      filter_parameters["bins_x"] = bins_x;
      filter_parameters["min_x"] = min_x;
      filter_parameters["max_x"] = max_x;
      filter_parameters["time_lower_range"] = time_lower_range;
      filter_parameters["time_upper_range"] = time_upper_range;

      // Check if we are running on MC or real data
      // Set time unit conversion
      if (gROOT->GetListOfGlobals()->FindObject("rawConv") == 0){
	isMC_ = true;
      } else {
	isMC_ = false;
      }

      // Get SciFi Digi hit collection
      scifiDigiHitCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndScifiHits"));
      if (! scifiDigiHitCollection) LOG(FATAL) << "ERROR: sndSciFiHitFilter could not find sndScifiHits";
	
      // Declare filtered hit collection. Use the interpreter to make it accessible from other SciFi classes
      gInterpreter->Declare("#include \"sndScifiHit.h\"");
      gInterpreter->Declare("TClonesArray scifiDigiHitCollection_filtered(\"sndScifiHit\", 3000);");
      scifiDigiHitCollection_filtered = static_cast<TClonesArray*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered"))->GetAddress());
      if (! scifiDigiHitCollection_filtered) {
	LOG(FATAL) << "ERROR: sndSciFiHitFilter could not find scifiDigiHitCollection_filtered";
      }
    }

    void sciFiHitFilter::process(){
      scifiDigiHitCollection_filtered->Clear("C");

      if(scifiDigiHitCollection->GetEntries() > 0){
	// Should make 0 and "TI18" constructor arguments?
	temp_clonesarray_ = std::move(snd::analysis_tools::filterScifiHits(*scifiDigiHitCollection, filter_parameters, 0, "TI18", isMC_));
      
	int i_hit = 0;
	for (TObject  *obj : *temp_clonesarray_){
	  sndScifiHit *hit = dynamic_cast<sndScifiHit*>(obj);

	  if (hit == nullptr){
	    LOG(FATAL) << "ERROR: sndScifiHitFilter process got null pointer from snd::analysis_tools::filterScifiHits";
	  }
	  (*scifiDigiHitCollection_filtered)[i_hit++] = hit;
	}
      }
    }
  }
}
