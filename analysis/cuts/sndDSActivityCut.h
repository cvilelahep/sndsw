#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace sndAnalysis {
  
  class DSActivityCut : public MuFilterBaseCut {
  public :
    DSActivityCut(TChain * tree);
    ~DSActivityCut(){;}
    bool passCut();
  };
}
