#include "dsp.h"
#include "filter.hpp"
#include "interpolation.hpp"
#include "verb.hpp"
#include "delay.hpp"

using namespace dspheaders;

float ChownVerb::play(float sample, float amount) {
  float sig = 0.f;
  int i = 0;
  for (i = 0; i < 4; i++) {
    sig += cvec[i].play(sample, ccoeff[i], COMBTYPE::IIR);
  }
  // rotate++;
  sig/=4;
  for (i = 0; i < 3; i++) {
    sig = avec[i].play(sig, amount);
  }
  return sig * amount;
}

ChownVerb::ChownVerb(unsigned samplerate) : samplerate(samplerate) {};

float SchroederVerb::play(float sample, float amount) {
  float sig = sample;
  int i = 0;
  for (i = 0; i < 3; i++) {
    sig = avec[i].play(sig, amount);
  }

  for (i = 0; i < 4; i++) {
    sig += cvec[i].play(sig, ccoeff[i] * amount, COMBTYPE::FIR);
  }

  return sig/5;
}

float SchroederVerb::play(float sample, float amount, float mod) {
  float sig = sample;
  int i = 0;
  for (i = 0; i < 3; i++) {
    sig = avec[i].play(sig, amount, mod);
  }

  for (i = 0; i < 4; i++) {
    sig += cvec[i].play(sig, ccoeff[i] * amount, mod, COMBTYPE::FIR);
  }
  return sig/5;
}

SchroederVerb::SchroederVerb(unsigned samplerate) : samplerate(samplerate) {};
