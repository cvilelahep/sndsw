#pragma once

#include "sndSciFiBaseCut.h"

#include "TChain.h"
#include "sndScifiHit.h"

#include "Scifi.h"
#include "TVector3.h"

namespace sndAnalysis {
  class scifiAngle : public sciFiBaseCut {
  private :
    std::pair<int, int> rXZ, rYZ;
    Scifi * scifiDet;
    TVector3 a, b;
  public :
    scifiAngle(std::pair<int, int> rangeXZ, std::pair<int, int> rangeYZ, TChain * tree);
    ~scifiAngle(){;}

    bool passCut();

  };
};
