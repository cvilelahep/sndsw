#pragma once

#include "sndBaseCut.h"

#include "TClonesArray.h"
#include "Scifi.h"

namespace snd {
  namespace analysis_cuts {

    class trackSciFiDOCA : public snd::analysis_cuts::baseCut {
      
    private :
      TClonesArray * recoTracksCollection;
      TClonesArray * scifiDigiHitCollection;
      Scifi * scifiDet;
      float sum_min_doca_cut_;
      bool all_tracks_;
    public :
      trackSciFiDOCA(float sum_min_doca_cut, bool all_tracks);
      void process();
    };
  }
}
      
