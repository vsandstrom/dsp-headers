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
        Comb2(&f0[0], samplerate, 1116, .75),
        Comb2(&f1[0], samplerate, 1188, .75),
        Comb2(&f2[0], samplerate, 1277, .75),
        Comb2(&f3[0], samplerate, 1356, 0.75),
      };


      float f4[556];
      float f5[441];
      float f6[341];

      Comb2 avec[3] = {
        Comb2(&f4[0], samplerate, 556, 0.5, 0.5),
        Comb2(&f5[0], samplerate, 441, 0.5, 0.5),
        Comb2(&f6[0], samplerate, 341, 0.5, 0.5),
      };

    public:
      inline float process(float sample);
      Verb(unsigned samplerate);
  };

inline float Verb::process(float sample) {
  float out = 0.f;
  for (int i = 0; i < 4; i++) {
    out += cvec[i].process(sample);
  }
  for (int j = 0; j < 3; j++) {
    out = avec[j].process(out);
  }
  return out;
}

}
