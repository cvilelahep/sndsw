#pragma once

#include "sndFilterProcessBase.h"
          
#include <string>
#include <vector>

namespace snd {
  namespace analysis_cuts{
    class baseCut: public snd::analysis_core::baseProcess {
    protected :
      std::string shortName;
      std::vector<int> nbins;
      std::vector<double> range_start;
      std::vector<double> range_end;
      std::vector<double> plot_var;
      bool passed_cut;

    public :
      bool passCut(){return passed_cut;}

      // For histograms
      std::string getShortName() {return shortName;}
      std::vector<int> getNbins() {return nbins;}
      std::vector<double> getRangeStart() {return range_start;}
      std::vector<double> getRangeEnd() {return range_end;}
      std::vector<double> getPlotVar() {return plot_var;}
    };

  }
}
