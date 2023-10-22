#pragma once
#include "dsp.h"
#include "filter.hpp"
#include "wavetable.hpp"

namespace dspheaders{
  class ChownVerb {
    private:
      unsigned rotate = 0;
      unsigned samplerate;
      Comb cvec[4] = {
        Comb(901, samplerate),
        Comb(778, samplerate),
        Comb(1011, samplerate),
        Comb(1123, samplerate),
      };

      float ccoeff[4] = {
        .805f, 
        .827f, 
        .783f, 
        .764f, 
      };

      Allpass avec[3] = {
        Allpass(125, samplerate),
        Allpass(42, samplerate),
        Allpass(12, samplerate),
      };

    public:
      float process(float sample, float amount);
      ChownVerb(unsigned samplerate);
  };
}
