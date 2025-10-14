#include "sndIP1.h"

#include "SNDLHCEventHeaderConst.h"

#include <iostream>

#include "FairLogger.h"

namespace snd::analysis_cuts {

  ip1Cut::ip1Cut() : EventHeaderBaseCut() {
    processName = "IP1 bunch crossing";

    shortName = "IP1";

    /* Plot variable encoded like this:
       0 isIP1();
       1 isB1Only();
       2 isB2noB1();
    */
    nbins = std::vector<int>{3};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{3};
    plot_var = std::vector<double>{-1};

  }

  void ip1Cut::process(){

    if (header->GetBunchType() == -1){
      LOG(FATAL) << "ERROR ip1Cut: Bunch type not set";
    }
    
    passed_cut = false;
    if (header->isIP1()){
      passed_cut = true;
      plot_var[0] = 0;
    } else if (header->isB1Only()){
      plot_var[0] = 1;
    } else if (header->isB2noB1()){
      plot_var[0] = 2;
    }
  }
}
