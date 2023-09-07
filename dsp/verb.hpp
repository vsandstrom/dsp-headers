
#pragma once
#include "dsp.h"
#include "buffer.hpp"
#include "delay.hpp"

namespace dspheaders{
  class Verb {
    private:
      float samplerate;
      Delay* delaylines;
      int readptr = 0;
      float prev[2] = {0.f, 0.f};
      float k[4]   { 0.8f, 0.2f, 0.34f, 0.12f };
      int i[4]   { 17, 19,  29, 47  };

      void write(float sample);
      float read();

    public:
      Verb(unsigned samplerate, float feedback, float (*interpolate)(float, float*, unsigned));
      float feedback;

      float play(float sample);
  };
}
