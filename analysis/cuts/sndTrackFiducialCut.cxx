#include "sndTrackFiducialCut.h"

#include "sndRecoTrack.h"

#include "TROOT.h"
#include "TVector3.h"

#include "FairLogger.h"

namespace snd::analysis_cuts{

  trackFiducialCut::trackFiducialCut(float z_extrap, float x_min, float x_max, float y_min, float y_max, bool all_tracks) : z_extrap_(z_extrap), x_min_(x_min), x_max_(x_max), y_min_(y_min), y_max_(y_max), all_tracks_(all_tracks)
  {
    
    processName = "Track extrapolates to fiducial volume";

    shortName = "trackFiducial";
    nbins = std::vector<int>{100, 100};

    float x_centre = (x_min_ + x_max_)/2.;
    float x_half_width = (x_max_ - x_min_)/2.;

    float y_centre = (y_min_ + y_max_)/2.;
    float y_half_width = (y_max_ - y_min_)/2.;
    
    range_start = std::vector<double>{x_centre - x_half_width*2, y_centre - y_half_width*2};
    range_end = std::vector<double>{x_centre + x_half_width*2, y_centre + y_half_width*2};
    plot_var = std::vector<double>{-1, -1};

    recoTracksCollection = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("sndRecoTracks"));
    if (!recoTracksCollection){
      LOG(FATAL) << "trackFiducialCut ERROR: Could not find sndRecoTracks";
    }
  };
  
  void trackFiducialCut::process(){

    int n_tracks = recoTracksCollection->GetEntries();

    if (n_tracks == 0){
      plot_var[0] = -9999;
      plot_var[1] = -9999;
      if (all_tracks_) passed_cut = false;
      else passed_cut = true;
      return;
    } else {
      std::vector<bool> tracks_in_fiducial;
      for (TObject * obj : *recoTracksCollection){
	sndRecoTrack * track = dynamic_cast<sndRecoTrack*>(obj);

	TVector3 track_start = track->getStart();
	TVector3 track_stop = track->getStop();

	float slope_X = (track_stop.X() - track_start.X())/(track_stop.Z() - track_start.Z());
	float slope_Y = (track_stop.Y() - track_start.Y())/(track_stop.Z() - track_start.Z());

	TVector3 track_extrap(track_start.X() + slope_X*(z_extrap_ - track_start.Z()),
			      track_start.Y() + slope_Y*(z_extrap_ - track_start.z()),
			      z_extrap_);

	plot_var[0] = track_extrap.X();
	plot_var[1] = track_extrap.Y();
	
	bool this_track_in_fiducial = true;
	if (track_extrap.X() > x_max_) {
	  this_track_in_fiducial = false;
	} else if (track_extrap.X() < x_min_) {
	  this_track_in_fiducial = false;
	} else if (track_extrap.Y() > y_max_) {
	  this_track_in_fiducial = false;
	} else if (track_extrap.Y() < y_min_) {
	  this_track_in_fiducial = false;
	}
	tracks_in_fiducial.push_back(this_track_in_fiducial);
	// No point looking at the rest if all tracks are required to be in the fiducial volume
	if ((not this_track_in_fiducial) and all_tracks_) break;
      }
      
      if(all_tracks_) {
	passed_cut = std::all_of(tracks_in_fiducial.begin(), tracks_in_fiducial.end(), [](bool v) { return v; });
      } else {
	passed_cut = std::any_of(tracks_in_fiducial.begin(), tracks_in_fiducial.end(), [](bool v) { return v; });
      }
    }
  }
}
