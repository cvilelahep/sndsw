#include "sndSciFiBaseCut.h"

#include <vector>

#include "TClonesArray.h"
#include "TChain.h"
#include "sndScifiHit.h"
#include "Scifi.h"
#include "TROOT.h"
#include "SNDLHCEventHeader.h"

namespace snd::analysis_cuts {

  TChain * sciFiBaseCut::tree = 0;
  SNDLHCEventHeader * sciFiBaseCut::header = 0;
  unsigned long int sciFiBaseCut::read_entry = -1;

  TClonesArray * sciFiBaseCut::scifiDigiHitCollection = 0;
  
  std::vector<int> sciFiBaseCut::hits_per_plane_vertical = std::vector<int>(5, 0);
  std::vector<int> sciFiBaseCut::hits_per_plane_horizontal = std::vector<int>(5, 0);

  Scifi * sciFiBaseCut::scifiDet = 0;
  
  sciFiBaseCut::sciFiBaseCut(TChain * ch){

    if (tree == 0){
      tree = ch;
      scifiDigiHitCollection = new TClonesArray("sndScifiHit", 3000);
      tree->SetBranchAddress("Digi_ScifiHits", &scifiDigiHitCollection);

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

      // Clear hits per plane vectors
      std::fill(hits_per_plane_vertical.begin(), hits_per_plane_vertical.end(), 0);
      std::fill(hits_per_plane_horizontal.begin(), hits_per_plane_horizontal.end(), 0);

      // Add valid hits to hits per plane vectors
      sndScifiHit * hit;
      TIter hitIterator(scifiDigiHitCollection);

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
    header = NULL;
  };
}
