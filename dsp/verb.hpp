#pragma once
#include "filter.hpp"
#include "interpolation.hpp"

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
      float process(float sample, float len);
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
      float process(float sample, float amount);
      float process(float sample, float amount, float mod);
      SchroederVerb(unsigned samplerate);
  };

  class MoorerVerb {
    private:
      unsigned samplerate;
      Allpass initial = Allpass(6, samplerate, interpolation::linear);
      Comb cvec[6] = {
        Comb(2400, samplerate, interpolation::linear),
        Comb(2688, samplerate, interpolation::linear),
        Comb(2928, samplerate, interpolation::linear),
        Comb(3264, samplerate, interpolation::linear),
        Comb(3455, samplerate, interpolation::linear),
        Comb(3744, samplerate, interpolation::linear),
      };
      
      float ccoeff[6] = {
        .742f, 
        .733f, 
        .715f, 
        .697f, 
        .683f, 
        .677f, 
      };
     
    public:
      float process(float sample, float length);
      MoorerVerb(unsigned samplerate);
  };
}
