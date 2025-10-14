#include "TROOT.h"

#include "sndFilterProcessBase.h"

#include "sndSciFiHitFilter.h"

#include "sndNtrackCut.h"
#include "sndEventDirectionCut.h"
#include "sndTrackFiducialCut.h"
#include "sndTrackSciFiDOCA.h"
#include "sndMaxDShits.h"

#include "sndCutFlowHist.h"

bool isMC = true;
if (gROOT->GetListOfGlobals()->FindObject("rawConv")) isMC = false;

std::vector< snd::analysis_core::baseProcess * > pipeline;

pipeline.push_back(new snd::analysis_processes::sciFiHitFilter(0.5*6.25, 1.2*6.25, 52, 0, 26)); // Select SciFi hits within [-0.5, 1.2] clock cycles of peak time

// Cuts related to DS tracking
pipeline.push_back(new snd::analysis_cuts::maxDShits(10, true));
pipeline.push_back(new snd::analysis_cuts::nTrackCut(1)); // At least one track
pipeline.push_back(new snd::analysis_cuts::eventDirectionCut()); // Event direction from IP1, using SciFi and DS hits
pipeline.push_back(new snd::analysis_cuts::trackFiducialCut(300, -8-39+5, -8-5, 15.5+5, 15.5+39-5, true)); // Track must be 5 cm from edge of SciFi at z = 300 cm
pipeline.push_back(new snd::analysis_cuts::trackSciFiDOCA(3, true));

// This creates all the cut flow histograms. Must be added after all the cuts.
pipeline.push_back( new snd::analysis_processes::cutFlowHist());   

