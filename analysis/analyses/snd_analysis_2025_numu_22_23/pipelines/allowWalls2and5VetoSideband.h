#include "TROOT.h"

#include "sndFilterProcessBase.h"

#include "sndStableBeams.h"
#include "sndIP1.h"
#include "sndEventDeltat.h"

#include "sndAvgSciFiFiducialCut.h"
#include "sndAvgDSFiducialCut.h"
#include "sndVetoCut.h"
#include "sndSciFiStationCut.h"
#include "sndDSActivityCut.h"
#include "sndMinSciFiConsecutivePlanes.h"
#include "sndPrescale.h"

#include "sndCutFlowHist.h"

bool isMC = true;
if (gROOT->GetListOfGlobals()->FindObject("rawConv")) isMC = false;

std::vector< snd::analysis_core::baseProcess * > pipeline;

if (not isMC) {
  pipeline.push_back( new snd::analysis_cuts::preScale(0.001) ); // Save only 1 in 1000 events.
  pipeline.push_back( new snd::analysis_cuts::stableBeamsCut());
  pipeline.push_back( new snd::analysis_cuts::ip1Cut());
  pipeline.push_back( new snd::analysis_cuts::eventDeltatCut(-1, 100)); // J. Previous event more than 100 clock cycles away. To avoid deadtime issues.
 }
pipeline.push_back( new snd::analysis_cuts::avgSciFiFiducialCut(200, 1200, 300, 128*12-200)); // E. Average SciFi hit channel number must be within [200, 1200] (ver) and [300, max-200] (hor)
pipeline.push_back( new snd::analysis_cuts::avgDSFiducialCut(70, 105, 10, 50)); // F. Average DS hit bar number must be within [70, 105] (ver) and [10, 50] (hor)
pipeline.push_back( new snd::analysis_cuts::DSActivityCut()); // H. If there is a downstream hit, require hits in all upstream stations.    
// This creates all the cut flow histograms. Must be added after all the cuts.
pipeline.push_back( new snd::analysis_processes::cutFlowHist());
