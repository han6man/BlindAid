#pragma once

#include "DepthObstacle.h"
#include "Capture.h"

namespace Vision
{
  namespace DepthObstacle
  {
    namespace FixedRegions
    {
      class FixedRegions : public Base
      {
      public:
        FixedRegions(IParameters * params, IData * input, IData * output);

      private:
        void Process();

      };
    }
  }
}