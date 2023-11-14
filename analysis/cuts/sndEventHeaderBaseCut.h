#pragma once

#include <vector>

#include "sndBaseCut.h"

#include "SNDLHCEventHeader.h"
#include "TChain.h"

namespace sndAnalysis {
  
  class EventHeaderBaseCut : public baseCut {

  protected :
    static SNDLHCEventHeader * header;
    static TChain * ch;

    EventHeaderBaseCut(TChain * ch);
    ~EventHeaderBaseCut(){;}
  };
}
