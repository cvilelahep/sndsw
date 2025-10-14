#include "TROOT.h"

#include "sndFilterProcessBase.h"

#include "sndSciFiHitFilter_legacy.h"
#include "sndNtrackCut.h"
#include "sndEventDirectionCut.h"
#include "sndTrackFiducialCut.h"
#include "sndTrackSciFiDOCA.h"
#include "sndMinSciFiHitsCut.h"
#include "sndUSQDCCut.h"
#include "sndMaxDShits.h"

#include "sndCutFlowHist.h"

bool isMC = true;
if (gROOT->GetListOfGlobals()->FindObject("rawConv")) isMC = false;

std::vector< snd::analysis_core::baseProcess * > pipeline;

pipeline.push_back(new snd::analysis_processes::sciFiHitFilter_legacy(-0.5, 1.2)); // Select SciFi hits within [-0.5, 1.2] clock cycles of peak time
pipeline.push_back(new snd::analysis_cuts::nTrackCut(1)); // At least one track
pipeline.push_back(new snd::analysis_cuts::eventDirectionCut()); // Event direction from IP1, using SciFi and DS hits
pipeline.push_back(new snd::analysis_cuts::trackFiducialCut(300, -8-39+5, -8-5, 15.5+5, 15.5+39-5, true)); // Track must be 5 cm from edge of SciFi at z = 300 cm
pipeline.push_back(new snd::analysis_cuts::trackSciFiDOCA(3, true));
pipeline.push_back(new snd::analysis_cuts::minSciFiHits(35));
if (isMC) pipeline.push_back( new snd::analysis_cuts::USQDCCut(700)); // Min QDC                                                                              
 else      pipeline.push_back( new snd::analysis_cuts::USQDCCut(600)); //
pipeline.push_back(new snd::analysis_cuts::maxDShits(10, true));

// This creates all the cut flow histograms. Must be added after all the cuts.                                                                                
pipeline.push_back( new snd::analysis_processes::cutFlowHist());   

