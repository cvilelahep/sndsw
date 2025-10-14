#pragma once

#include "sndSciFiBaseCut.h"

#include "TChain.h"
#include "sndScifiHit.h"

namespace snd {
  namespace analysis_cuts {
    class avgSciFiFiducialCut : public snd::analysis_cuts::sciFiBaseCut {
    private :
      double vertical_min_, vertical_max_, horizontal_min_, horizontal_max_;
      bool reversed_;
    public :
      avgSciFiFiducialCut(double vertical_min, double vertical_max, double horizontal_min, double horizontal_max, bool reversed = false);
      ~avgSciFiFiducialCut(){;}

      void process();
    };

  }
}
