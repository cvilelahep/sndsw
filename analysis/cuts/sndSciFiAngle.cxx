#include "sndSciFiAngle.h"

#include "sndSciFiTools.h"

#include "TChain.h"

#include "Scifi.h"
#include "TPython.h"
#include "TROOT.h"

#include "TVector3.h"

namespace sndAnalysis{
  scifiAngle::scifiAngle(std::pair<int, int> rangeXZ, std::pair<int, int> rangeYZ, TChain * tree) : sciFiBaseCut(tree){
    
    rXZ = rangeXZ;
    rYZ = rangeYZ;
    
    cutName = "SciFi angle";

    shortName = "SciFiAngle";
    nbins = std::vector<int>{100, 100};
    range_start = std::vector<double>{-50, -50};
    range_end = std::vector<double>{50, 50};
    plot_var = std::vector<double>{-1, -1};

    TPython::Exec("import SndlhcGeo");
    TPython::Exec("SndlhcGeo.GeoInterface('/eos/experiment/sndlhc/convertedData/physics/2023/geofile_sndlhc_TI18_V3_2023.root')");
    
    scifiDet = (Scifi*) gROOT->GetListOfGlobals()->FindObject("Scifi");
    
  }

  bool scifiAngle::passCut(){
    initializeEvent();
    //scifiDet->InitializeEvent(); NEED TO FIND A WAY TO DO THIS. FAIRROOT?!
    
    std::vector<double> pos_ver = std::vector<double>(5, 0.);
    
    std::vector<double> pos_hor = std::vector<double>(5, 0.);

    std::vector<double> pos_z = std::vector<double>(5, 0.);

    sndScifiHit * hit;
    TIter hitIterator(scifiDigiHitCollection);

    while ( (hit = (sndScifiHit*) hitIterator.Next()) ){
      if (hit->isValid()){

	scifiDet->GetSiPMPosition(hit->GetDetectorID(), a, b);

	int sta = hit->GetStation();
	if (hit->isVertical()){
	  pos_ver[sta-1] += (a.X() + b.X())/2.;
	} else {
	  pos_hor[sta-1] += (a.Y() + b.Y())/2.;
	}
	pos_z[sta-1] += (a.Z() + b.Z())/2.;
      }
    }

    // For a plane to count, need both planes to have hits
    //    for (int i_plane = 0; i_plane < hits_per_plane_horizontal.size(); i_plane++){
    //      if ((hits_per_plane_horizontal[i_plane]*hits_per_plane_vertical[i_plane] == 0) and gotFirstPlane){
    //	  return false;
    //      } else if (hits_per_plane_horizontal[i_plane]*hits_per_plane_vertical[i_plane] > 0){
    //	gotFirstPlane = true;
    //      }
    //    }
    return true;
  }
}	     
