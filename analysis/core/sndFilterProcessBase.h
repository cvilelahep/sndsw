#pragma once

#include <string>
#include <vector>

namespace snd {
  namespace analysis_core{
    class baseProcess{
    protected :
      std::string processName;
    public :
      virtual void process() = 0;
      virtual ~baseProcess() = default;
      std::string getName() {return processName;}

    };

  }
}
