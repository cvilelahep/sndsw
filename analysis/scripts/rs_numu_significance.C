#include "RooStats/RooStatsUtils.h"
#include <iostream>
 
using namespace RooFit;
using namespace RooStats; // the utilities are in the RooStats namespace
using std::cout, std::endl;
 
void rs_numu_significance()
{

  std::cout << "PRL 2023" << std::endl;
  double sExpected = 4.2;
  double bExpected = 8.6e-2;
  double relativeBkgUncert = 0.44;
 
  double pExp = NumberCountingUtils::BinomialExpP(sExpected, bExpected, relativeBkgUncert);
  double zExp = NumberCountingUtils::BinomialExpZ(sExpected, bExpected, relativeBkgUncert);
  cout << "expected p-value =" << pExp << "  Z value (Gaussian sigma) = " << zExp << endl;
  
  // -------------------------------------------------
  // Expected p-values and significance with background uncertainty
  double observed = 8;
  double pObs = NumberCountingUtils::BinomialObsP(observed, bExpected, relativeBkgUncert);
  double zObs = NumberCountingUtils::BinomialObsZ(observed, bExpected, relativeBkgUncert);
  cout << "observed p-value =" << pObs << "  Z value (Gaussian sigma) = " << zObs << endl;

  std::cout << "\nMoriond 2024" << std::endl;
  sExpected = 19;
  bExpected = 0.25;
  relativeBkgUncert = 0.44;
 
  pExp = NumberCountingUtils::BinomialExpP(sExpected, bExpected, relativeBkgUncert);
  zExp = NumberCountingUtils::BinomialExpZ(sExpected, bExpected, relativeBkgUncert);
  cout << "expected p-value =" << pExp << "  Z value (Gaussian sigma) = " << zExp << endl;
  
  // -------------------------------------------------
  // Expected p-values and significance with background uncertainty
  observed = 32;
  pObs = NumberCountingUtils::BinomialObsP(observed, bExpected, relativeBkgUncert);
  zObs = NumberCountingUtils::BinomialObsZ(observed, bExpected, relativeBkgUncert);
  cout << "observed p-value =" << pObs << "  Z value (Gaussian sigma) = " << zObs << endl;
}
