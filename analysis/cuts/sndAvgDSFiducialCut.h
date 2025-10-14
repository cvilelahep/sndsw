#pragma once

#include "sndMuFilterBaseCut.h"

#include "TChain.h"
#include "MuFilterHit.h"

namespace snd {
  namespace analysis_cuts {
    class avgDSFiducialCut : public snd::analysis_cuts::MuFilterBaseCut {
    private :
      double vertical_min_, vertical_max_, horizontal_min_, horizontal_max_;
    public :
      avgDSFiducialCut(double vertical_min, double vertical_max, double horizontal_min, double horizontal_max);
      ~avgDSFiducialCut(){;}

      void process();

    };

  }
}
