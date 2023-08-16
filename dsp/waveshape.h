#include "dsp.h"
#include <cmath>

using namespace dspheaders;

inline void sine(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = pi * 2.f / numsamples;
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = sin(angle);
    angle += inc;
  }
}


inline void saw(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 2.0 / (numsamples-1);
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = angle - 1.f;
    angle += inc;
  }
}

inline void square(float* table, unsigned tablelength) {
  float val = 1.f;
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = val;
    if (i == tablelength/2 - 1) {
      val = -1.f;
    }
  }
}

inline void hanning(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = pi / numsamples;
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = powf(sin(angle), 2.f);
    angle += inc;
  }
}

inline void triangle(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 2.f / (numsamples * 0.5);
  for (unsigned i = 0; i < tablelength; i++) {
    if (angle >= 1.f || angle <= -1.f) {
      inc *= -1.0;
    }
    table[i] = angle;
    angle += inc;
  }
}

// TODO: Can use phase
// Create a Wavetable of a bunch of partials of sines, based on their amplitudes and phases
//
inline float* complex_sine(
    float* table,
    unsigned tablelength, 
    float* amplitudes,
    unsigned amplength,
    float* phases
    ) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;

  // Make sure amplitudes are within 0 & 1
  scale(amplitudes, amplength, 0.f, 1.f);

  // where n = 1
  // inc = pi * 2.f * n / numsamples;
  // positionphase = inc + (numsamples * inc); 
  // value = positionphase * amplitude
  
  for (int n=1; n <= amplength; n++) {
    inc = pi * 2.f * n / numsamples;
    angle = inc * numsamples * phases[n-1];

    for (int i=0; i<tablelength; i++) {
      table[i] += (sin(angle) * amplitudes[n-1]);
      angle += inc;
    }
  }

  scale(table, tablelength, -1.f, 1.f);
  return table;
}
