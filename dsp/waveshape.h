
#include "dsp.h"
#include "dsp_math.h"
#include <cmath>
#include <cstddef>

#pragma once

using namespace dspheaders;
inline void sine(float* table, size_t tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = PI * 2.f / numsamples;
  for (size_t i = 0; i < tablelength; i++) {
    table[i] = sin(angle);
    angle += inc;
  }
}


inline void saw(float* table, size_t tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 2.0 / (numsamples-1);
  for (size_t i = 0; i < tablelength; i++) {
    table[i] = angle - 1.f;
    angle += inc;
  }
}

inline void revsaw(float* table, size_t tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 2.0 / (numsamples-1);
  for (size_t i = 0; i < tablelength; i++) {
    table[i] = angle + 1.f;
    angle -= inc;
  }
}

inline void square(float* table, size_t tablelength) {
  float val = -1.f;
  for (size_t i = 0; i < tablelength; i++) {
    table[i] = val;
    if (i == tablelength/2 - 1) {
      val = 1.f;
    }
  }
}

inline void phasor(float* table, size_t tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 1.0 / (numsamples-1);
  for (size_t i = 0; i < tablelength; i++) {
    table[i] = angle;
    angle += inc;
  }
}

inline void revsquare(float* table, size_t tablelength) {
  float val = 1.f;
  for (size_t i = 0; i < tablelength; i++) {
    table[i] = val;
    if (i == (tablelength / 2)-1) {
      val=-1.f;
    }
  } 
}

inline void hanning(float* table, size_t tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = PI / numsamples;
  for (size_t i = 0; i < tablelength; i++) {
    table[i] = powf(sin(angle), 2.f);
    angle += inc;
  }
  return table;
}

inline void triangle(float* table, size_t tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 2.f / (numsamples * 0.5);
  for (size_t i = 0; i < tablelength; i++) {
    if (angle >= 1.f || angle <= -1.f) {
      inc *= -1.0;
    }
    table[i] = angle;
    angle += inc;
  }
}

///
/// Creates a additive waveshape based on partials of sines with
/// different amplitudes and phases
///

inline float* complex_sine(
    float* table,
    size_t tablelength, 
    float* amplitudes,
    float* phases,
    size_t paramslength
    ) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;

  // Make sure amplitudes are within 0 & 1
  // scale(amplitudes, paramslength, 0.f, 1.f);
  // normalize(amplitudes, paramslength);

  // ------------ FORMULA --------------------
  // where n = 1 -> paramslength
  // inc = PI * 2.f * n / numsamples;
  // positionphase = inc + (numsamples * inc); 
  // value = positionphase * amplitude
  // -----------------------------------------
  
  for (int n=1; n <= paramslength; n++) {
    inc = PI * 2.f * n / numsamples;
    angle = inc * numsamples * phases[n-1];

    // magic extra length because of Buffer shenanigans
    for (size_t i=0; i<tablelength+1; i++) {
      table[i] += (sin(angle) * amplitudes[n-1]);
      angle += inc;
    }
  }

  // more shenanigans 
  scale(table, tablelength+1, -1.f, 1.f);
  // return for simplicity, the function mutates array in place through pointer.
  return table;
}
