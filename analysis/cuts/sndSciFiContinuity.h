#pragma once

#include "sndSciFiBaseCut.h"

#include "TChain.h"
#include "sndScifiHit.h"

namespace sndAnalysis {
  class sciFiContinuity : public sciFiBaseCut {
  public :
    sciFiContinuity(TChain * tree);
    ~sciFiContinuity(){;}

    bool passCut();

  };
};
