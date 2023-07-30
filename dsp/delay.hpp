#pragma once
#include "dsp.h"
#include "buffer.hpp"

namespace dspheaders {
  class Delay {
    private:
      Buffer buffer;
      unsigned samplerate;
      unsigned delay_taps = 1;
      float time;
      int writeptr = 0;

      float read(float delaytime);
      void write(float sample);

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

  };
}
