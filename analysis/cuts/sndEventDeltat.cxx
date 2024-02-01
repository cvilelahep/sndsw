#include "sndEventDeltat.h"

#include <iostream>

namespace snd::analysis_cuts {

  eventDeltatCut::eventDeltatCut(int delta_event, int delta_timestamp, TChain * tree) : EventHeaderBaseCut(tree) {
    delta_e = delta_event;
    delta_t = delta_timestamp;

    cutName = std::to_string(delta_e)+" event more than "+std::to_string(delta_t)+" clock cycles away";

    shortName = "EventDeltat_";
    shortName += std::to_string(delta_event);
    shortName += "_";
    shortName += std::to_string(delta_timestamp);
    nbins = std::vector<int>{1000};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{1000};
    plot_var = std::vector<double>{-1};

  }

  bool eventDeltatCut::passCut(){
    unsigned long int current_entry = ch->GetReadEntry();
    long int current_time = header->GetEventTime();

    bool passes = true;
    ch->GetEntry(current_entry + delta_e);

    int sign = (delta_e > 0) - (delta_e < 0);

    if (-sign*(current_time - header->GetEventTime()) <= delta_t) passes = false;

    plot_var[0] = abs(current_time - header->GetEventTime());
    
    // Get current entry back
    ch->GetEntry(current_entry);
    return passes;
  }
}
