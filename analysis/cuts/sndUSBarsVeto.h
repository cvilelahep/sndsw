#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace sndAnalysis {
  
  class USBarsVeto : public MuFilterBaseCut {
  private :
    std::vector<std::pair<int, int> > bars;
  public :
    USBarsVeto(std::vector<std::pair<int, int> > bars_to_exclude, TChain * tree);
    ~USBarsVeto(){;}
    bool passCut();
  };
}
