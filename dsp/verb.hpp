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


  class SchroederVerb {
    private:
      unsigned rotate = 0;
      unsigned samplerate;
      Comb cvec[4] = {
        Comb(4799, samplerate, interpolation::linear),
        Comb(4999, samplerate, interpolation::linear),
        Comb(5399, samplerate, interpolation::linear),
        Comb(5801, samplerate, interpolation::linear),
      };

      float ccoeff[4] = {
        .742f, 
        .733f, 
        .715f, 
        .697f, 
      };

      Allpass avec[3] = {
        Allpass(1051, samplerate, interpolation::linear),
        Allpass(337, samplerate, interpolation::linear),
        Allpass(113, samplerate, interpolation::linear),
      };
    public:
      float play(float sample, float amount);
      float play(float sample, float amount, float mod);
      SchroederVerb(unsigned samplerate);
  };
}
