#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace snd {
  namespace analysis_cuts {

    class maxDShits : public snd::analysis_cuts::MuFilterBaseCut {
    private:
      float max_hits_;
      bool avg_layer_;
    public :
      maxDShits(float max_hits, bool avg_layer);
      ~maxDShits(){;}
      void process();
    };

  }
}
