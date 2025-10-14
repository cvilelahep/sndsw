#include "sndMuFilterBaseCut.h"

#include <vector>

#include "TROOT.h"
#include "TClonesArray.h"
#include "MuFilterHit.h"

namespace snd::analysis_cuts {

  MuFilterBaseCut::MuFilterBaseCut(){
    muFilterDigiHitCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("MuFilterHits"));
  }
}
