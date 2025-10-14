#pragma once

#include "sndFilterProcessBase.h"

#include "TH1D.h"

#include "Scifi.h"

namespace snd{
  namespace analysis_processes{
    class sciFiHitFilter_legacy : public snd::analysis_core::baseProcess {
    public:
      sciFiHitFilter_legacy(double min_clock_cycle, double max_clock_cycle);
      void process();
    private:
      double min_clock_cycle_;
      double max_clock_cycle_;
      double TDC2ns_;
      bool isMC_;

      std::vector<TH1D *> * hHitTime;

      TClonesArray * scifiDigiHitCollection;
      TClonesArray * scifiDigiHitCollection_filtered;

      Scifi * scifiDet;
    };
  }
}
      
