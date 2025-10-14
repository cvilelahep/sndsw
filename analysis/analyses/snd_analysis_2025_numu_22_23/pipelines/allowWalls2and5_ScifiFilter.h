#include "TROOT.h"

#include "sndFilterProcessBase.h"

#include "sndSciFiHitFilter.h"

#include "sndStableBeams.h"
#include "sndIP1.h"
#include "sndEventDeltat.h"

#include "sndAvgSciFiFiducialCut.h"
#include "sndAvgDSFiducialCut.h"
#include "sndVetoCut.h"
#include "sndSciFiStationCut.h"
#include "sndMinSciFiHitsCut.h"
#include "sndUSQDCCut.h"
#include "sndDSActivityCut.h"

#include "sndCutFlowHist.h"

bool isMC = true;
if (gROOT->GetListOfGlobals()->FindObject("rawConv")) isMC = false;

std::vector< snd::analysis_core::baseProcess * > pipeline;

// CUts related to event header
if (not isMC) {
  pipeline.push_back( new snd::analysis_cuts::stableBeamsCut());
  pipeline.push_back( new snd::analysis_cuts::ip1Cut());
  pipeline.push_back( new snd::analysis_cuts::eventDeltatCut(-1, 100)); // J. Previous event more than 100 clock cycles away. To avoid deadtime issues.
 }

// SciFi hit selection
//      sciFiHitFilter(float time_lower_range, float time_upper_range, float bins_x, float min_x, float max_x);

pipeline.push_back(new snd::analysis_processes::sciFiHitFilter(0.5*6.25, 1.2*6.25, 52, 0, 26)); // Select SciFi hits within [-0.5, 1.2] clock cycles of peak time

// Fiduvial volume cuts
pipeline.push_back( new snd::analysis_cuts::avgSciFiFiducialCut(200, 1200, 300, 128*12-200)); // E. Average SciFi hit channel number must be within [200, 1200] (ver) and [300, max-200] (hor)
pipeline.push_back( new snd::analysis_cuts::avgDSFiducialCut(70, 105, 10, 50)); // F. Average DS hit bar number must be within [70, 105] (ver) and [10, 50] (hor)
pipeline.push_back( new snd::analysis_cuts::vetoCut()); // B. No veto hits
pipeline.push_back( new snd::analysis_cuts::sciFiStationCut(0., std::vector<int>(1, 1))); // C. No hits in first SciFi plane

// Detector activity cuts
pipeline.push_back(new snd::analysis_cuts::minSciFiHits(35));
if (isMC) pipeline.push_back( new snd::analysis_cuts::USQDCCut(700)); // Min QDC
 else      pipeline.push_back( new snd::analysis_cuts::USQDCCut(600)); //

pipeline.push_back( new snd::analysis_cuts::DSActivityCut()); // H. If there is a downstream hit, require hits in all upstream stations.    

// This creates all the cut flow histograms. Must be added after all the cuts.
pipeline.push_back( new snd::analysis_processes::cutFlowHist());
