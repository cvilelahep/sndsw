#include "sndTrackSciFiDOCA.h"

#include <numeric>

#include "sndRecoTrack.h"
#include "sndScifiHit.h"
#include "Scifi.h"

#include "TROOT.h"
#include "TGlobal.h"
#include "TVector3.h"

#include "FairLogger.h"

namespace snd::analysis_cuts{

  trackSciFiDOCA::trackSciFiDOCA(float sum_min_doca_cut, bool all_tracks) : sum_min_doca_cut_(sum_min_doca_cut), all_tracks_(all_tracks)
  {
    processName = "Track distance of closest approach to SciFi hits";

    shortName = "trackSciFiDoca";
    nbins = std::vector<int>{100};

    range_start = std::vector<double>{0};
    range_end = std::vector<double>{500};
    plot_var = std::vector<double>{-1};

    scifiDigiHitCollection = nullptr;
    if (dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered")) != nullptr){
      scifiDigiHitCollection = static_cast<TClonesArray*>(dynamic_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("scifiDigiHitCollection_filtered"))->GetAddress());
    }
    if (scifiDigiHitCollection == nullptr){
      scifiDigiHitCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndScifiHits"));
    }
    if (scifiDigiHitCollection == nullptr){
      LOG(FATAL) << "eventDirectionCut ERROR: Could not find sndScifiHits";
    }
    
    recoTracksCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndRecoTracks"));
    if (!recoTracksCollection){
      LOG(FATAL) << "trackSciFiDOCA ERROR: Could not find sndRecoTracks";
    }

    // Get detector object
    scifiDet = dynamic_cast<Scifi*>(gROOT->GetListOfGlobals()->FindObject("Scifi"));

  };
  
  void trackSciFiDOCA::process(){

    int n_tracks = recoTracksCollection->GetEntries();

    if (n_tracks == 0){
      plot_var[0] = -9999;

      if (all_tracks_) passed_cut = false;
      else passed_cut = true;

      return;
      
    } else {
      TVector3 a, b;
      
      std::vector<float> tracks_sum_min_doca;
      for (TObject * track_obj : *recoTracksCollection){
	sndRecoTrack * track = dynamic_cast<sndRecoTrack*>(track_obj);

	TVector3 track_start = track->getStart();
	TVector3 track_stop = track->getStop();

	float slope_X = (track_stop.X() - track_start.X())/(track_stop.Z() - track_start.Z());
	float slope_Y = (track_stop.Y() - track_start.Y())/(track_stop.Z() - track_start.Z());

	std::vector<float> min_doca_ver(5, 1e10);
	std::vector<float> min_doca_hor(5, 1e10);

	for (TObject * hit_obj : *scifiDigiHitCollection){
	  sndScifiHit * hit = dynamic_cast<sndScifiHit*>(hit_obj);
	  if (not hit->isValid()) continue;

	  scifiDet->GetSiPMPosition(hit->GetDetectorID(), a, b);
	  float slope_XY, track_start_XY, hit_pos[2];
	  if (hit->isVertical()){
	    slope_XY = slope_X;
	    track_start_XY = track_start.X();
	    hit_pos[0] = (a.Z() + b.Z())/2.;
	    hit_pos[1] = (a.X() + b.X())/2.;
	  } else {
	    slope_XY = slope_Y;
	    track_start_XY = track_start.Y();
	    hit_pos[0] = (a.Z() + b.Z())/2.;
	    hit_pos[1] = (a.Y() + b.Y())/2.;
	  }
	  
	  float this_d = std::abs(track_start_XY + slope_XY*(hit_pos[0]-track_start.Z()) - hit_pos[1]);

	  if (hit->isVertical()){
	    if (this_d < min_doca_ver[hit->GetStation()-1]) min_doca_ver[hit->GetStation()-1] = this_d;
	  } else {
	    if (this_d < min_doca_hor[hit->GetStation()-1]) min_doca_hor[hit->GetStation()-1] = this_d;
	  }
	}

	int n_ver = 5 - std::count_if(min_doca_ver.begin(), min_doca_ver.end(),
				      [](float f) { return f > 9.9e9; });
	int n_hor = 5 - std::count_if(min_doca_hor.begin(), min_doca_hor.end(),
				      [](float f) { return f > 9.9e9; });

	if (n_ver == 0){
	  LOG(DEBUG) << "Track SciFi DOCA cut called but there are no vertical SciFi hits";
	  plot_var[0] = 999;
	  passed_cut = false;
	  return;
	}

	if (n_hor == 0){
	  LOG(DEBUG) << "Track SciFi DOCA cut called but there are no horizontal SciFi hits";
	  plot_var[0] = 999;
	  passed_cut = false;
	  return;
	}

	float avg_ver = std::accumulate(min_doca_ver.begin(), min_doca_ver.end(), static_cast<float>(0.),
					[](float sum, float element){return element < 9.9e9 ? sum+element: sum;})/static_cast<float>(n_ver);
	float avg_hor = std::accumulate(min_doca_hor.begin(), min_doca_hor.end(), static_cast<float>(0.),
					[](float sum, float element){return element < 9.9e9 ? sum+element: sum;})/static_cast<float>(n_hor);
	  
	tracks_sum_min_doca.push_back(std::max(avg_ver, avg_hor));
      }

      if (all_tracks_){
	plot_var[0] = *std::max_element(tracks_sum_min_doca.begin(), tracks_sum_min_doca.end());
      } else {
	plot_var[0] = *std::min_element(tracks_sum_min_doca.begin(), tracks_sum_min_doca.end());
      }
      
      passed_cut = plot_var[0] <= sum_min_doca_cut_;
    }
  }
}

