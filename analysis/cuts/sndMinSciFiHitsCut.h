#pragma once

#include "sndSciFiBaseCut.h"

#include "TChain.h"
#include "sndScifiHit.h"

namespace snd {
  namespace analysis_cuts {
    class minSciFiHits : public snd::analysis_cuts::sciFiBaseCut {
    private :
      int hit_threshold_;
    public :
      minSciFiHits(int hit_threshold);
      ~minSciFiHits(){;}

      void process();

    };

  }
}
