#include "sndEventHeaderBaseCut.h"

#include <stdexcept>

#include "SNDLHCEventHeader.h"

#include "TROOT.h"
#include "TChain.h"

#include <iostream>

namespace snd::analysis_cuts {
  
  EventHeaderBaseCut::EventHeaderBaseCut(){
    tree = dynamic_cast<TChain*>(gROOT->GetListOfGlobals()->FindObject("rawConv"));
    if (tree == 0) tree = dynamic_cast<TChain*>(gROOT->GetListOfGlobals()->FindObject("cbmsim"));
    
    header = dynamic_cast<SNDLHCEventHeader*>(gROOT->GetListOfGlobals()->FindObject("EventHeader"));
    if (header == 0) header = dynamic_cast<SNDLHCEventHeader*>(gROOT->GetListOfGlobals()->FindObject("EventHeader."));
    
  }
}

