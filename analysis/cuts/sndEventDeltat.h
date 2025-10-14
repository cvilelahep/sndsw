#pragma once

#include "sndEventHeaderBaseCut.h"

#include "TChain.h"

namespace snd {
  namespace analysis_cuts {
    class eventDeltatCut : public snd::analysis_cuts::EventHeaderBaseCut {
    private:
      int delta_event_;
      int delta_timestamp_;
    public :
      eventDeltatCut(int delta_event, int delta_timestamp);
      ~eventDeltatCut(){;}
      void process();
    };
  }
}
