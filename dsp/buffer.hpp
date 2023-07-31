#pragma once
#include "dsp.h"
#include "interpolation.hpp"
#include <cmath>
#include <iostream>

/*
 * TODO: 
 * [  ] - Make buffers resize itself if atk/rel-values in Envelope changes for example
 */
namespace dspheaders {
// Self-wrapping and interpolating Buffer
  class Buffer{
    private:
      float (*interpolate)(float, float*, unsigned);
    public:
      float* buffer;
      unsigned bufferlength;
      float readsample(float readptr);
      void writesample(float sample, int writeptr);
      void initbuffer();
      Buffer(
        float seconds,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
      
      Buffer(
        unsigned samples,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
  };
}
