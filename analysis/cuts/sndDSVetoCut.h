#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace snd {
  namespace analysis_cuts {

    class DSVetoCut : public snd::analysis_cuts::MuFilterBaseCut {
    public :
      DSVetoCut();
      ~DSVetoCut(){;}
      void process();
    };

  }
}
