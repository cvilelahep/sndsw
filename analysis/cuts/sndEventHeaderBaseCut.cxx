#include "sndEventHeaderBaseCut.h"

#include <stdexcept>

#include "SNDLHCEventHeader.h"
#include "TChain.h"

namespace sndAnalysis {

  SNDLHCEventHeader * EventHeaderBaseCut::header = 0;
  TChain * EventHeaderBaseCut::ch = 0;

  EventHeaderBaseCut::EventHeaderBaseCut(TChain * tree){
    if (header == 0){
      header = new SNDLHCEventHeader();
      tree->SetBranchAddress("EventHeader", &header);
      tree->GetEntry(0);
      if (header->GetEventTime() == -1) {
	tree->SetBranchAddress("EventHeader.", &header);
	tree->GetEntry(0);
	if (header->GetEventTime() == -1) throw std::runtime_error("Invalid event header");
      }
      ch = tree;
    }
  }
}
