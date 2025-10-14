#pragma once

#include "sndFilterProcessBase.h"
#include "sndBaseCut.h"

#include <vector>
#include "TH1D.h"
#include "TClonesArray.h"

namespace snd{
  namespace analysis_processes{
    class cutFlowHist : public snd::analysis_core::baseProcess {
    public:
      cutFlowHist();
      void process();
    private :

      bool isMC;

      TClonesArray * MCTracks;
      
      std::vector< snd::analysis_cuts::baseCut * > * cuts;
      
      std::vector<std::vector<TH1D*> > * cut_by_cut_var_histos;
      std::vector<TH1D*> * cutFlow_by_species;
      std::vector<std::vector<std::vector<TH1D*> > > * cut_by_cut_truth_histos;
      std::vector<TH1D*> * n_minus_1_var_histos;
      TH1D * cutFlowHistogram;

      unsigned int getSpecies(TClonesArray* tracks);
    };
  }
}
