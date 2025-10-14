#include "sndPrescale.h"

#include <random>

namespace snd::analysis_cuts{

  preScale::preScale(double fraction): fraction_(fraction) {
    
    gen = std::mt19937(r());
    dis = std::uniform_real_distribution<double>(0., 1.);
  };

  void preScale::process(){
    if (dis(gen) < fraction_) {passed_cut = true; return;}
    else {passed_cut = false; return;}
  };
}
