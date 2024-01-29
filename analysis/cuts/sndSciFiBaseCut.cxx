#include "sndSciFiBaseCut.h"

#include <vector>

#include "TClonesArray.h"
#include "TChain.h"
#include "sndScifiHit.h"

namespace sndAnalysis {

  TClonesArray * sciFiBaseCut::scifiDigiHitCollection = 0;
  TChain * sciFiBaseCut::ch = 0;
  unsigned long int sciFiBaseCut::read_entry = -1;

  std::vector<int> sciFiBaseCut::hits_per_plane_vertical = std::vector<int>(5, 0);
  std::vector<int> sciFiBaseCut::hits_per_plane_horizontal = std::vector<int>(5, 0);

  sciFiBaseCut::sciFiBaseCut(TChain * tree){
    if (ch == 0){
      ch = tree;
      scifiDigiHitCollection = new TClonesArray("sndScifiHit", 3000);
      ch->SetBranchAddress("Digi_ScifiHits", &scifiDigiHitCollection);
    }
  }

  void sciFiBaseCut::initializeEvent(){
    if (read_entry != ch->GetReadEntry()){
      read_entry = ch->GetReadEntry();
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
}
