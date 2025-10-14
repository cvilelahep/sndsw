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
#include "sndUSPlanesHit.h"
#include "sndUSBarsVeto.h"
#include "sndUSQDCCut.h"
#include "sndDSVetoCut.h"
#include "sndSciFiContinuity.h"
#include "sndMinSciFiHitsCut.h"

#include "sndCutFlowHist.h"

bool isMC = true;
if (gROOT->GetListOfGlobals()->FindObject("rawConv")) isMC = false;

std::vector< snd::analysis_core::baseProcess * > pipeline;

if (not isMC) {
  pipeline.push_back( new snd::analysis_cuts::stableBeamsCut());
  pipeline.push_back( new snd::analysis_cuts::ip1Cut());
  pipeline.push_back( new snd::analysis_cuts::eventDeltatCut(-1, 100)); // J. Previous event more than 100 clock cycles away. To avoid deadtime issues.
 }
pipeline.push_back( new snd::analysis_cuts::vetoCut()); // B. No veto hits
pipeline.push_back( new snd::analysis_cuts::avgSciFiFiducialCut(200, 1200, 300, 128*12-200)); // E. Average SciFi hit channel number must be within [200, 1200] (ver) and [300, max-200] (hor)
pipeline.push_back( new snd::analysis_cuts::DSVetoCut()); // D. Veto events with hits in last DS planes
pipeline.push_back( new snd::analysis_cuts::minSciFiConsecutivePlanes()); // G. At least two consecutive SciFi planes hit
pipeline.push_back( new snd::analysis_cuts::minSciFiHits(35)); // At least 35 SciFi hits
if (isMC) pipeline.push_back( new snd::analysis_cuts::USQDCCut(700)); // Min QDC
 else      pipeline.push_back( new snd::analysis_cuts::USQDCCut(600)); // 
pipeline.push_back( new snd::analysis_cuts::sciFiContinuity()); // All SciFi planes downstream of first active (both views) plane must be hit (both views).
pipeline.push_back( new snd::analysis_cuts::USPlanesHit(std::vector<int>{0, 1})); // All SciFi planes downstream of first active (both views) plane must be hit (both views).    
pipeline.push_back( new snd::analysis_cuts::USBarsVeto(std::vector<std::pair<int, double> >{{0, 2.}, {1, 2.}}, true)); // Reject events with hits in lowest bars in first two US planes
pipeline.push_back( new snd::analysis_cuts::USBarsVeto(std::vector<std::pair<int, double> >{{0, 8.}, {1, 8.}}, false)); // Reject events with hits in highest bars in first two US planes
// This creates all the cut flow histograms. Must be added after all the cuts.
pipeline.push_back( new snd::analysis_processes::cutFlowHist());
