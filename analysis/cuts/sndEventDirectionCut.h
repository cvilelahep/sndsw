#pragma once

#include "sndBaseCut.h"

#include "TClonesArray.h"
#include "Scifi.h"

namespace snd {
  namespace analysis_cuts {
  
    class eventDirectionCut : public snd::analysis_cuts::baseCut {

    private :
      TClonesArray * muFilterDigiHitCollection;
      TClonesArray * scifiDigiHitCollection;

      bool B1_;
      double delta_t_;

      bool isMC_;
      bool TDC2ns_;

      Scifi * scifiDet;
      //MuFilter * muFilterDet;
      
    public :
      eventDirectionCut(bool B1 = true, double delta_t = 0);
      void process();
    };
  }
}
