#include "sndSciFiHitFilter_legacy.h"

#include "TROOT.h"
#include "TGlobal.h"
#include "TInterpreter.h"
#include "TH1D.h"

#include "Scifi.h"
#include "sndScifiHit.h"
#include "ShipUnit.h"

#include "FairLogger.h"

namespace snd{
  namespace analysis_processes{
    sciFiHitFilter_legacy::sciFiHitFilter_legacy(double min_clock_cycle = -0.5, double max_clock_cycle = 1.2){
      
      // Check if we are running on MC or real data
      // Set time unit conversion
      if (gROOT->GetListOfGlobals()->FindObject("rawConv") == 0){
	TDC2ns_ = 1;
	isMC_ = true;
      } else {
	TDC2ns_ = ShipUnit::snd_TDC2ns;
	isMC_ = false;
      }
    
      // Get parameters from constructor
      min_clock_cycle_ = min_clock_cycle;
      max_clock_cycle_ = max_clock_cycle;
    
      // Get SciFi Digi hit collection
      scifiDigiHitCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndScifiHits"));
      if (! scifiDigiHitCollection) LOG(FATAL) << "ERROR: sndSciFiHitFilter_legacy could not find sndScifiHits";
	
      // Declare filtered hit collection. Use the interpreter to make it accessible from other SciFi classes
      gInterpreter->Declare("#include \"sndScifiHit.h\"");
      gInterpreter->Declare("TClonesArray scifiDigiHitCollection_filtered(\"sndScifiHit\", 3000);");
      scifiDigiHitCollection_filtered = static_cast<TClonesArray*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered"))->GetAddress());
      if (! scifiDigiHitCollection_filtered) {
	LOG(FATAL) << "ERROR: sndSciFiHitFilter_legacy could not find scifiDigiHitCollection_filtered";
      }
    
      // Set up histograms for finding the SciFi peak hit time.
      // NOTE this is a legacy version, for reproducibility only.
      // USE SCIFITOOLS VERSION FOR NEW ANALYSES!
      hHitTime = new std::vector<TH1D*>();
      for (int i_orientation = 0; i_orientation < 2; i_orientation++){
	for (int i_station = 0; i_station < 5; i_station++){
	  hHitTime->push_back(new TH1D(("hHitTime_"+std::to_string(i_orientation)+"_"+std::to_string(i_station)).c_str(), ";SciFi hit time [clock cycles]", 200., 0., 100.));
	}
      }

      // Get detector object
      scifiDet = dynamic_cast<Scifi*>(gROOT->GetListOfGlobals()->FindObject("Scifi"));
    }

    void sciFiHitFilter_legacy::process(){

      for (int i_orientation = 0; i_orientation < 2; i_orientation++){
	for (int i_station = 0; i_station < 5; i_station++){
	  hHitTime->at(i_orientation*5+i_station)->Reset();
	}
      }

      scifiDigiHitCollection_filtered->Clear();

      for (TObject * obj : *scifiDigiHitCollection){
	sndScifiHit * hit = dynamic_cast<sndScifiHit*>(obj);
	if (hit->isValid()){
	  int this_orientation = 0;
	  if (hit->isVertical()) this_orientation = 1;
	  int this_station = hit->GetStation() - 1;
	  // Get corrected time!!!
	  double time = hit->GetTime()*TDC2ns_;
	  if (!isMC_) time = scifiDet->GetCorrectedTime(hit->GetDetectorID(), time, 0);
	  hHitTime->at(this_orientation*5+this_station)->Fill(time);
	}
      }
      std::vector<double> peakTiming(2*5, 0);
    
      for (int i_orientation = 0; i_orientation < 2; i_orientation++){
	for (int i_station = 0; i_station < 5; i_station++){
	  int i = i_orientation*5+i_station;
	  peakTiming.at(i) = (hHitTime->at(i)->GetMaximumBin()-0.5)*(hHitTime->at(i)->GetXaxis()->GetXmax()-hHitTime->at(i)->GetXaxis()->GetXmin())/hHitTime->at(i)->GetNbinsX() + hHitTime->at(i)->GetXaxis()->GetXmin();
	}
      }

      int i_hit = 0;
      for (TObject * obj : *scifiDigiHitCollection){
	sndScifiHit * hit = dynamic_cast<sndScifiHit*>(obj);
	if (not hit->isValid()) continue;
	double time = hit->GetTime()*TDC2ns_;
	if (!isMC_) time = scifiDet->GetCorrectedTime(hit->GetDetectorID(), time, 0);
	int this_orientation = 0;
	if (hit->isVertical()) this_orientation = 1;
	int this_station = hit->GetStation() - 1;
	int i = this_orientation*5+this_station;
        
	if (time < peakTiming.at(i) + min_clock_cycle_*TDC2ns_) continue;
	if (time > peakTiming.at(i) + max_clock_cycle_*TDC2ns_) continue;
	(*scifiDigiHitCollection_filtered)[i_hit++] = hit;
      }
    }
  }
}
