#pragma once

#include <vector>

#include "sndBaseCut.h"

#include "SNDLHCEventHeader.h"
#include "TChain.h"

namespace snd {
  namespace analysis_cuts {
  
    class EventHeaderBaseCut : public snd::analysis_cuts::baseCut {

    protected :
      TChain * tree;
      SNDLHCEventHeader * header;

      EventHeaderBaseCut();
      ~EventHeaderBaseCut(){;}
    };

  }
}
