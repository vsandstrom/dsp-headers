#include "dsp.h"
#include "interpolation.hpp"
#include "verb.hpp"
#include "delay.hpp"

using namespace dspheaders;

float ChownVerb::process(float sample, float amount) {
  float sig = 0.f;
  for (int i = 0; i < 4; i++) {
    sig += cvec[i].play(sample, ccoeff[i], -ccoeff[i]);
  }
  for (int j = 0; j < 3; j++) {
    sig = avec[j].play2(sig, 0.7);
  }
  return sig;
}

ChownVerb::ChownVerb(unsigned samplerate) : samplerate(samplerate) {};
