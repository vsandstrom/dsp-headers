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
