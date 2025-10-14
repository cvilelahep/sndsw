#pragma once

#include "sndFilterProcessBase.h"

#include "TH1D.h"

#include "Scifi.h"

namespace snd{
  namespace analysis_processes{
    class sciFiHitFilter : public snd::analysis_core::baseProcess {
    public:
      sciFiHitFilter(float time_lower_range, float time_upper_range, float bins_x, float min_x, float max_x);
      void process();
    private:
      bool isMC_;

      std::unique_ptr<TClonesArray> temp_clonesarray_;
      
      std::map<std::string, float> filter_parameters;
      
      TClonesArray * scifiDigiHitCollection;
      TClonesArray * scifiDigiHitCollection_filtered;
    };
  }
}
      
