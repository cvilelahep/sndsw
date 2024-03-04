#include "sndSciFiBaseCut.h"

#include <vector>

#include "TClonesArray.h"
#include "TChain.h"
#include "sndScifiHit.h"
#include "Scifi.h"
#include "TROOT.h"
#include "TH1D.h"
#include "SNDLHCEventHeader.h"

namespace snd::analysis_cuts {

  TChain * sciFiBaseCut::tree = 0;
  SNDLHCEventHeader * sciFiBaseCut::header = 0;
  unsigned long int sciFiBaseCut::read_entry = -1;
  
  TClonesArray * sciFiBaseCut::scifiDigiHitCollection = 0;
  TClonesArray * sciFiBaseCut::scifiDigiHitCollection_raw = 0;
  TH1D * sciFiBaseCut::hHitTime = 0;
  
  std::vector<int> sciFiBaseCut::hits_per_plane_vertical = std::vector<int>(5, 0);
  std::vector<int> sciFiBaseCut::hits_per_plane_horizontal = std::vector<int>(5, 0);

  Scifi * sciFiBaseCut::scifiDet = 0;
  
  sciFiBaseCut::sciFiBaseCut(TChain * ch, bool select_events, double min_clock_cycle, double max_clock_cycle){

    if (tree == 0){
      tree = ch;

      select_events_ = select_events;
      min_clock_cycle_ = min_clock_cycle;
      max_clock_cycle_ = max_clock_cycle;
      
      if (not select_events_) {
	scifiDigiHitCollection = new TClonesArray("sndScifiHit", 3000);
	tree->SetBranchAddress("Digi_ScifiHits", &scifiDigiHitCollection);
      }
      else {
	scifiDigiHitCollection_raw = new TClonesArray("sndScifiHit", 3000);
	tree->SetBranchAddress("Digi_ScifiHits", &scifiDigiHitCollection_raw);
	
	scifiDigiHitCollection = new TClonesArray("sndScifiHit", 3000);
	hHitTime = new TH1D("hHitTime", ";SciFi hit time [clock cycles]", 52., 0., 26.);
      }
      
      scifiDet = (Scifi*) gROOT->GetListOfGlobals()->FindObject("Scifi");

      if (header == 0){
	// Check if branch exists in the TTree
	if (tree->FindBranch("EventHeader") != NULL){
	  if (tree->FindBranch("EventHeader")->GetAddress() != NULL) {
	    header = (SNDLHCEventHeader *) *((char**)tree->FindBranch("EventHeader")->GetAddress());
	    tree->GetEntry(0);
	    // Invalid runId. Invalid object?
	    if (header->GetRunId() == 0) header = NULL;
	  }
	}

	// Check alternate branch name
	if (tree->FindBranch("EventHeader.") != NULL and header == NULL){
	  if (tree->FindBranch("EventHeader.")->GetAddress() != NULL) {
	    header = (SNDLHCEventHeader *) *((char**) tree->FindBranch("EventHeader.")->GetAddress());
	    tree->GetEntry(0);
	    // Invalid runId. Invalid object?
	    if (header->GetRunId() == 0) header = NULL;
	  }
	}

	// Header is still null, create object and set address
	if (header == NULL) {
	  header = new SNDLHCEventHeader();
	  tree->SetBranchAddress("EventHeader", &header);
	  tree->GetEntry(0);
	  if (header->GetEventTime() == -1) {
	    tree->SetBranchAddress("EventHeader.", &header);
	    tree->GetEntry(0);
	    if (header->GetEventTime() == -1) throw std::runtime_error("Invalid event header");
	  }
	}
      }
    }
  }


  void sciFiBaseCut::initializeEvent(){
    if (read_entry != tree->GetReadEntry()){
      read_entry = tree->GetReadEntry();
      
      // Initialize detector
      scifiDet->InitEvent(header);

      sndScifiHit * hit;
      
      TIter hitIterator(scifiDigiHitCollection);
      TIter hitIterator_raw(scifiDigiHitCollection_raw);

      if (select_events_){
	hHitTime->Reset();
	scifiDigiHitCollection->Clear();
	
	while (hit = (sndScifiHit*) hitIterator_raw.Next()){
	  if (hit->isValid()){

	    // Get corrected time!!!
	    double time = hit->GetTime()*1E9/160.316E6;
	    time = scifiDet->GetCorrectedTime(hit->GetDetectorID(), time, 0);
	    hHitTime->Fill(time);
	  }
	}

	double peakTiming = (hHitTime->GetMaximumBin()-0.5)*(hHitTime->GetXaxis()->GetXmax()-hHitTime->GetXaxis()->GetXmin())/hHitTime->GetNbinsX() + hHitTime->GetXaxis()->GetXmin();

	hitIterator_raw.Reset();
	int i_hit = 0;
	while (hit = (sndScifiHit*) hitIterator_raw.Next()){
	  if (not hit->isValid()) continue;
	  double time = hit->GetTime()*1E9/160.316E6;
	  time = scifiDet->GetCorrectedTime(hit->GetDetectorID(), time, 0);
	  if (time < peakTiming + min_clock_cycle_*1E9/160.316E6) continue;
	  if (time > peakTiming + max_clock_cycle_*1E9/160.316E6) continue;
	  (*scifiDigiHitCollection)[i_hit++] = hit;
	}
      }

      // Clear hits per plane vectors
      std::fill(hits_per_plane_vertical.begin(), hits_per_plane_vertical.end(), 0);
      std::fill(hits_per_plane_horizontal.begin(), hits_per_plane_horizontal.end(), 0);
      
      // Add valid hits to hits per plane vectors
      hitIterator.Reset();
      while ( (hit = (sndScifiHit*) hitIterator.Next()) ){
	if (hit->isValid()){
	  int sta = hit->GetStation();
	  if (hit->isVertical()){
	    hits_per_plane_vertical[sta-1]++;
	  } else {
	    hits_per_plane_horizontal[sta-1]++;
	  }
	}
      }
    }
  }
  sciFiBaseCut::~sciFiBaseCut(){
    delete scifiDigiHitCollection;
    delete scifiDigiHitCollection_raw;
    header = NULL;
  };
}
