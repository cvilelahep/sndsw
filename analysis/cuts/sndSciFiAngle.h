#pragma once

#include "sndSciFiBaseCut.h"

#include "TChain.h"
#include "Scifi.h"

#include "TVector3.h"
#include "TGraph.h"

namespace snd {
  namespace analysis_cuts {

    class scifiAngle : public sciFiBaseCut {
    private :
      double intercept_, slope_, max_chi2_;
      TVector3 a_, b_;
      TGraph * gv_;
      TGraph * gh_;
      Scifi * scifiDet;
    public :
      scifiAngle(double intercept, double slope, double max_chi2);
      ~scifiAngle(){;}

      void process();

    };
  };
}
