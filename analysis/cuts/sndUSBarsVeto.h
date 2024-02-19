#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace snd {
  namespace analysis_cuts{
  
    class USBarsVeto : public MuFilterBaseCut {
    private :
      std::vector<std::pair<int, double> > bars;
    public :
      USBarsVeto(std::vector<std::pair<int, double> > avg_per_plane, TChain * tree);
      ~USBarsVeto(){;}
      bool passCut();
    };
  }
}
