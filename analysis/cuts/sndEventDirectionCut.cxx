#include "sndEventDirectionCut.h"

#include <algorithm>

#include "TROOT.h"
#include "TGlobal.h"
#include "TClonesArray.h"

#include "MuFilterHit.h"
#include "sndScifiHit.h"
#include "Scifi.h"
#include "ShipUnit.h"

#include "FairLogger.h"

namespace snd::analysis_cuts{
  eventDirectionCut::eventDirectionCut(bool B1, double delta_t) : B1_(B1), delta_t_(delta_t){

    processName = "Event in";
    if (B1_) processName += " B1";
    else processName += "B2";
    processName += " direction. Delta t = " + std::to_string(delta_t_) + " ns";

    nbins = std::vector<int>{200};
    range_start = std::vector<double>{-10};
    range_end = std::vector<double>{10};
    plot_var = std::vector<double>{-1};
    
    muFilterDigiHitCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("MuFilterHits"));;
    if (!muFilterDigiHitCollection){
      LOG(FATAL) << "eventDirectionCut ERROR: Could not find MuFilterHits";
    }

    scifiDigiHitCollection = nullptr;
    if (dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered")) != nullptr){
      scifiDigiHitCollection = static_cast<TClonesArray*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered"))->GetAddress());
    }
    if (scifiDigiHitCollection == nullptr){
      scifiDigiHitCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndScifiHits"));
    }
    if (scifiDigiHitCollection == nullptr){
      LOG(FATAL) << "eventDirectionCut ERROR: Could not find sndScifiHits";
    }

    // Check if we are running on MC or real data
    // Set time unit conversion
    if (gROOT->GetListOfGlobals()->FindObject("rawConv") == 0){
      TDC2ns_ = 1;
      //      isMC_ = true;
    } else {
      TDC2ns_ = ShipUnit::snd_TDC2ns;
      //      isMC_ = false;
    }

    // Get detector object
    scifiDet = dynamic_cast<Scifi*>(gROOT->GetListOfGlobals()->FindObject("Scifi"));
    //    muFilterDet = dynamic_cast<MuFilter*>(gROOT->GetListOfGlobals()->FindObject("MuFilter"));
  };
  
  void eventDirectionCut::process(){

    std::vector<float> scifi_times;
    std::vector<float> ds_times;

    for (TObject * obj : *scifiDigiHitCollection){
      sndScifiHit * hit = dynamic_cast<sndScifiHit*>(obj);
      float t = hit->GetTime()*TDC2ns_;
      // Should use corrected time!
      // if (not isMC_) t = scifiDet->GetCorrectedTime(hit->GetDetectorID(), t, 0);
      scifi_times.push_back(t);
    }
    
    for (TObject * obj : *muFilterDigiHitCollection){
      MuFilterHit * hit = dynamic_cast<MuFilterHit*>(obj);
      if (hit->GetSystem() != 3) continue;
      float t;
      // Get time correction
      if (hit->isVertical()){
	t = hit->GetTime(0)*TDC2ns_;
      } else {
	t = (hit->GetTime(0) + hit->GetTime(1))/2*TDC2ns_;
      }
      ds_times.push_back(t);
    }

    float min_t, max_t;
    if (scifi_times.size() == 0){
      LOG(DEBUG) << "Event direction cut called, but there are no SciFi hits in the event";
      plot_var[0] = -999;
      passed_cut = false;
      return;
    } else {
      min_t = *std::min_element(scifi_times.begin(), scifi_times.end());
    }
    if (ds_times.size() == 0){
      LOG(DEBUG) << "Event direction cut called, but there are no DS hits in the event";
      plot_var[0] = -999;
      passed_cut = false;
      return;
    } else {
      max_t = *std::max_element(ds_times.begin(), ds_times.end());
    }

    float t_diff = max_t - min_t;

    plot_var[0] = t_diff;
    
    if (B1_) passed_cut = t_diff > delta_t_;
    else passed_cut = -1*t_diff > delta_t_;
  }
}
