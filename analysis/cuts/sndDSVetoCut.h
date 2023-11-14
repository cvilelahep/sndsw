#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace sndAnalysis {
  
  class DSVetoCut : public MuFilterBaseCut {
  public :
    DSVetoCut(TChain * tree);
    ~DSVetoCut(){;}
    bool passCut();
  };
}
