#include "sndEventDeltat.h"

#include <iostream>

namespace snd::analysis_cuts {

  eventDeltatCut::eventDeltatCut(int delta_event, int delta_timestamp) : EventHeaderBaseCut(), delta_event_(delta_event), delta_timestamp_(delta_timestamp) {

    processName = std::to_string(delta_event_)+" event more than "+std::to_string(delta_timestamp_)+" clock cycles away";

    shortName = "EventDeltat_";
    shortName += std::to_string(delta_event_);
    shortName += "_";
    shortName += std::to_string(delta_timestamp_);
    nbins = std::vector<int>{1000};
    range_start = std::vector<double>{0};
    range_end = std::vector<double>{1000};
    plot_var = std::vector<double>{-1};
  }

  void eventDeltatCut::process(){
    unsigned long int current_entry = tree->GetReadEntry();
    long int current_time = header->GetEventTime();

    bool passes = true;
    tree->GetEntry(current_entry + delta_event_);

    int sign = (delta_event_ > 0) - (delta_event_ < 0);

    if (-sign*(current_time - header->GetEventTime()) <= delta_timestamp_) passes = false;

    plot_var[0] = abs(current_time - header->GetEventTime());
    
    // Get current entry back
    tree->GetEntry(current_entry);
    passed_cut = passes;
  }
}
