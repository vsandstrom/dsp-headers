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
        Comb(901, samplerate, interpolation::linear),
        Comb(778, samplerate, interpolation::linear),
        Comb(1011, samplerate, interpolation::linear),
        Comb(1123, samplerate, interpolation::linear),
      };

      float ccoeff[4] = {
        .805f, 
        .827f, 
        .783f, 
        .764f, 
      };

      Allpass avec[3] = {
        Allpass(125, samplerate, interpolation::linear),
        Allpass(42, samplerate, interpolation::linear),
        Allpass(12, samplerate, interpolation::linear),
      };

    public:
      float play(float sample, float amount);
      ChownVerb(unsigned samplerate);
  };
}
