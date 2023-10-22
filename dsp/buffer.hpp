#pragma once
#include "dsp.h"
#include "interpolation.hpp"
#include <cmath>

namespace dspheaders {

  class Buffer{
    public:
      float *buffer;
      unsigned bufferlength;

      // Read a sample of a floored position value
      const float readsample(unsigned readptr);

      const float * getReadPtr(unsigned readptr);
        
      // write a sample to a position in the buffer
      void writesample(float sample, int writeptr);

      void init();
      
      // Initialize buffer based on a duration in samples
      Buffer(
        unsigned samples,
        unsigned samplerate
      );

      ~Buffer();
  };
  
}
