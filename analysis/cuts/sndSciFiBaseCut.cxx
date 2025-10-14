#include "sndSciFiBaseCut.h"

#include <vector>

#include "TGlobal.h"
#include "TClonesArray.h"
#include "TChain.h"
#include "sndScifiHit.h"
#include "Scifi.h"
#include "TROOT.h"
#include "TInterpreter.h"
#include "TGlobal.h"
#include "TH1D.h"
#include "ShipUnit.h"

#include "FairLogger.h"

namespace snd::analysis_cuts {
  sciFiBaseCut::sciFiBaseCut(){

    // Use global variable to keep track of the read entry
    if(!gROOT->GetListOfGlobals()->FindObject("sndSciFiBaseReadEntry")){
      gInterpreter->Declare("long long int sndSciFiBaseReadEntry = -1;");
    }
    read_entry = static_cast<long long int*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("sndSciFiBaseReadEntry"))->GetAddress());
    
    if (!gROOT->GetListOfGlobals()->FindObject("sndSciFiBaseHitsPerPlaneV")){
      gInterpreter->Declare("std::vector<int> sndSciFiBaseHitsPerPlaneV = std::vector<int>(5, 0);");
    }
    hits_per_plane_vertical = static_cast<std::vector<int>*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("sndSciFiBaseHitsPerPlaneV"))->GetAddress());
    
    if (!hits_per_plane_vertical){
      LOG(FATAL) << "Error sndSciFiBaseCut: couldn't get sndSciFiBaseHitsPerPlaneV";
    }
    if (!gROOT->GetListOfGlobals()->FindObject("sndSciFiBaseHitsPerPlaneH")){
      gInterpreter->Declare("std::vector<int> sndSciFiBaseHitsPerPlaneH = std::vector<int>(5, 0);");
    }
    hits_per_plane_horizontal = static_cast<std::vector<int>*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("sndSciFiBaseHitsPerPlaneH"))->GetAddress());
    if (!hits_per_plane_horizontal){
      LOG(FATAL) << "Error sndSciFiBaseCut: couldn't get sndSciFiBaseHitsPerPlaneH";
    }

    // Try to get the filtered hit collection. If it doesn't exist, use the raw collection.
    scifiDigiHitCollection = nullptr;
    if (dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered")) != nullptr){
      scifiDigiHitCollection = static_cast<TClonesArray*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered"))->GetAddress());
    }
    if (scifiDigiHitCollection == nullptr){
      LOG(INFO) << "sciFiBaseCut INFO: couldn't find filtered hit collection. Using all SciFi hits";
      scifiDigiHitCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndScifiHits"));
      if (scifiDigiHitCollection == nullptr){
	LOG(FATAL) << "sciFiBaseCut ERROR: couldn't find scifi hit collection";
      }
    }
  }

  void sciFiBaseCut::initializeEvent(){

    // Do this only once per event.
    // TODO: move this part of the code to a processor module and get rid of SciFiBaseCut.
    TTree * tree;
    tree = (TTree*) gROOT->GetListOfGlobals()->FindObject("rawConv");
    if (! tree) tree = (TTree*) gROOT->GetListOfGlobals()->FindObject("cbmsim");
    
    if ((*read_entry) != tree->GetReadEntry()){
      (*read_entry) = tree->GetReadEntry();

      // Clear hits per plane vectors
      std::fill(hits_per_plane_vertical->begin(), hits_per_plane_vertical->end(), 0);
      std::fill(hits_per_plane_horizontal->begin(), hits_per_plane_horizontal->end(), 0);
      
      // Add valid hits to hits per plane vectors
      for (TObject *obj : *scifiDigiHitCollection){
	sndScifiHit *hit = dynamic_cast<sndScifiHit*>(obj);
	
	if (hit == nullptr) LOG(FATAL) << "sndSciFiBaseCut got non-sndSciFiHit in collection";
	
	if (hit->isValid()){
	  int sta = hit->GetStation();
	  if (hit->isVertical()){
	    hits_per_plane_vertical->at(sta-1)++;
	  } else {
	    hits_per_plane_horizontal->at(sta-1)++;
	  }
	}
      }
    }
  }
}
