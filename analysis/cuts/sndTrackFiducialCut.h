#pragma once

#include "sndBaseCut.h"

#include "TClonesArray.h"

namespace snd {
  namespace analysis_cuts {

    class trackFiducialCut : public snd::analysis_cuts::baseCut {
      
    private :
      TClonesArray * recoTracksCollection;
      float z_extrap_;
      float x_min_;
      float x_max_;
      float y_min_;
      float y_max_;
      bool all_tracks_;
      
    public :
      trackFiducialCut(float z_extrap, float x_min, float x_max, float y_min, float y_max, bool all_tracks);
      void process();
    };
  }
}
      
