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
      float* buffer;
      float (*interpolate)(float, float*, unsigned);
    public:
      unsigned bufferlength;
      float readsample(float readptr);
      void writesample(float sample, int writeptr);
      void initbuffer();
      Buffer(
        float seconds,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
  };
}
