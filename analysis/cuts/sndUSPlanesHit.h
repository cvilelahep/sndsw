#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"

namespace snd{
  namespace analysis_cuts {
  
    class USPlanesHit : public MuFilterBaseCut {
    private :
      std::vector<int> planes_hit_;
    public :
      USPlanesHit(std::vector<int> planes_hit);
      ~USPlanesHit(){;}
      void process();
    };
    
  }
}
