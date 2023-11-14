#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace sndAnalysis {
  
  class vetoCut : public MuFilterBaseCut {
  public :
    vetoCut(TChain * tree);
    ~vetoCut(){;}
    bool passCut();
  };
}
