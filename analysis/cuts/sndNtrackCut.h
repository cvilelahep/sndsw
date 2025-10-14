#pragma once

#include "sndBaseCut.h"

#include "TClonesArray.h"

namespace snd {
  namespace analysis_cuts {
  
    class nTrackCut : public snd::analysis_cuts::baseCut {

    private :
      TClonesArray * recoTracksCollection;
      int n_max_;
      int n_min_;
      
    public :
      nTrackCut(int n_min = 0, int n_max = -1);
      void process();
    };
  }
}
      
