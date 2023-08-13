#pragma once
#include "dsp.h"
#include "buffer.hpp"

namespace dspheaders {
  class Delay {
    protected:
      Buffer buffer;
      unsigned samplerate;
      unsigned delay_taps = 1;
      float time = 0.2;
      int writeptr = 0;

      virtual float read(float delaytime);
      virtual void write(float sample);

    public:
      void taps(unsigned taps);
      void delaytime(float delaytime);
      float play(float input, float delaytime, float wet, float feedback);

      Delay(
        unsigned samplerate,
        float time,
        unsigned delay_taps,
        float (*interpolate)(float, float*, unsigned)
      );
      
      Delay(
        unsigned samplerate,
        float time,
        float (*interpolate)(float, float*, unsigned)
      );

  };
}
