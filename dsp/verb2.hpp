#pragma once
#include "dsp.h"
#include "comb2.hpp"
#include "wavetable.hpp"

namespace dspheaders{
  class Verb {
    private:
      unsigned rotate = 0;
      unsigned samplerate;
      float f0[1116];
      float f1[1188];
      float f2[1277];
      float f3[1356];

      Comb2 cvec[4] = {
        Comb2(&f0[0], samplerate, 1116, 0.95, 0),
        Comb2(&f1[0], samplerate, 1188, 0.95, 0),
        Comb2(&f2[0], samplerate, 1277, 0.95, 0),
        Comb2(&f3[0], samplerate, 1356, 0.95, 0),
      };


      float f4[125];
      float f5[42];
      float f6[13];

      Comb2 avec[3] = {
        Comb2(&f4[0], samplerate, 125, 0.7, 0.7),
        Comb2(&f5[0], samplerate, 42, 0.7, 0.7),
        Comb2(&f6[0], samplerate, 13, 0.7, 0.7),
      };

    public:
      inline float process(float sample);
      Verb(unsigned samplerate);
  };

inline float Verb::process(float in) {
  float out = 0.f;
  for (int i = 0; i < 4; i++) {
    out += cvec[i].process(in);
  }
  for (int j = 0; j < 3; j++) {
    out = avec[j].process(out);
  }
  return out;
}

}
