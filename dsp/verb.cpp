#include "filter.hpp"
#include <cstdlib>
#include "verb.hpp"
#include "dsp_math.h"

using namespace dspheaders;

float ChownVerb::process(float sample, float length) {

  float sig = 0.f;
  int i = 0;
  for (i = 0; i < 4; i++) {
    sig += cvec[i].process(sample, ccoeff[i], COMBTYPE::IIR);
  }


  // rotate++;
  sig/=4;
  for (i = 0; i < 3; i++) {
    sig = avec[i].process(sig, length);
  }
  return sig * length;
}

ChownVerb::ChownVerb(unsigned samplerate) : samplerate(samplerate) {
  for (int i = 0; i < 4; i++) cvec[i].setDamp(0.3f);
  for (int i = 0; i < 3; i++) avec[i].setDamp(0.0f);
};


float SchroederVerb::process(float sample, float amount) {
  float sig = sample;
  int i = 0;
  for (i = 0; i < 3; i++) {
    sig = avec[i].process(sig, amount);
  }

  for (i = 0; i < 4; i++) {
    sig += cvec[i].process(sig, ccoeff[i] * amount, COMBTYPE::FIR);
  }

  return sig/5;
}

float SchroederVerb::process(float sample, float amount, float mod) {
  float sig = sample;
  int i = 0;
  for (i = 0; i < 3; i++) {
    sig = avec[i].process(sig, amount, mod);
  }

  for (i = 0; i < 4; i++) {
    sig += cvec[i].process(sig, ccoeff[i] * amount, mod, COMBTYPE::FIR);
  }
  return sig/5;
}

SchroederVerb::SchroederVerb(unsigned samplerate) : samplerate(samplerate) {
  for (int i = 0; i < 4; i++) cvec[i].setDamp(0.3f);
  for (int i = 0; i < 3; i++) avec[i].setDamp(0.0f);
};

float MoorerVerb::process(float sample, float length) {
  float sig = sample;
  int i = 0;
  sig = initial.process(sig, 1);
  float out = 0.f;

  unsigned len = length * samplerate;

  for (i = 0; i < 6; i++) {
    float amount = dbtorms(
        (
         (
          (
           (float)cvec[i].getBufferLength() / samplerate
           ) * 1000
          ) * -60 / len
         ) + 100
        );
    out += cvec[i].process(sig, amount, COMBTYPE::IIR);
  }

  return sig;
}

MoorerVerb::MoorerVerb(unsigned samplerate) : samplerate(samplerate) {
  for (int i = 0; i < 6; i++) cvec[i].setDamp(0.1f);
};
