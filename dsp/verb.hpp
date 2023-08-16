
#pragma once
#include "dsp.h"
#include "buffer.hpp"
#include "delay.hpp"
#include <x86intrin.h>

namespace dspheaders{
  class Verb : public Delay{
    private:
      unsigned samplerate;
      float fb;
      // __m128 fb;
      int writeptr = 0;
      float v[4]   { 0,    0,     0,    0     };
      float k[4]   { 0.8f, 0.35f, 0.5f, 0.12f };
      float   i[4] { 17.f, 19.f,  29.f, 47.f  };

      // void write();
      // float read(int pos);

    public:
      Verb(unsigned samplerate, float feedback, float (*interpolate)(float, float*, unsigned));
      void feedback(double feedback);
      float play(float sample);
  };
}
