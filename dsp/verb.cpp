#include "dsp.h"
#include "interpolation.hpp"
#include "verb.hpp"
#include "delay.hpp"

using namespace dspheaders;

float ChownVerb::play(float sample, float amount) {
  float sig = 0.f;
  for (int i = 0; i < 4; i++) {
    sig += cvec[i].play(sample, ccoeff[(i+rotate)%4], COMBTYPE::IIR);
  }
  rotate++;
  sig/=4;
  for (int j = 0; j < 3; j++) {
    sig += avec[j].play(sig, 0.7);
  }
  return sig * amount;
}

ChownVerb::ChownVerb(unsigned samplerate) : samplerate(samplerate) {}
