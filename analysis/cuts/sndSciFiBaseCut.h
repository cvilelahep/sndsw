#pragma once

#include <vector>

#include "sndBaseCut.h"

#include "TChain.h"
#include "TClonesArray.h"
#include "sndScifiHit.h"
#include "Scifi.h"
#include "TH1D.h"

namespace snd {
  namespace analysis_cuts {
  
    class sciFiBaseCut : public snd::analysis_cuts::baseCut {

    private : 
      long long int * read_entry;
      
    protected :
      TClonesArray * scifiDigiHitCollection;
      
      std::vector<int> * hits_per_plane_vertical;
      std::vector<int> * hits_per_plane_horizontal;

      void initializeEvent();

      sciFiBaseCut();
    };

  }
}
