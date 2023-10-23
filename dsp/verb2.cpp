#include "dsp.h"
#include "verb2.hpp"

using namespace dspheaders;

Verb::Verb(unsigned samplerate) : samplerate(samplerate) {
  for (int i = 0; i < 4; i++) {
    cvec[i].setDamp(0.3);
  }
  for (int j = 0; j < 3; j++) {
    avec[j].setDamp(0.);
  }
};
