#pragma once

#include "dsp.h"
#include "dsp_math.h"
#include <cmath>

using namespace dspheaders;
constexpr void sine(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = TAU / numsamples;
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = sin_approx(angle, 10);
    angle += inc;
  }
}

constexpr void saw(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 2.0 / (numsamples-1);
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = angle - 1.f;
    angle += inc;
  }
}

constexpr void revsaw(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = 2.0 / (numsamples-1);
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = angle + 1.f;
    angle -= inc;
  }
}

constexpr void square(float* table, unsigned tablelength) {
  float val = -1.f;
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = val;
    if (i == tablelength/2 - 1) {
      val = 1.f;
    }
  }
}

constexpr void revsquare(float* table, unsigned tablelength) {
  float val = 1.f;
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = val;
    if (i == (tablelength / 2)-1) {
      val=-1.f;
    }
  } 
}

constexpr float* hanning(float* table, unsigned tablelength) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;
  inc = PI / numsamples;
  for (unsigned i = 0; i < tablelength; i++) {
    table[i] = powf_approx(sin_approx(angle, 10), 2.f);
    angle += inc;
  }
  return table;
}

constexpr void triangle(float* table, unsigned tablelength) {
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

///
/// Creates a additive waveshape based on partials of sines with
/// different amplitudes and phases
///

inline float* complex_sine(
    float* table,
    unsigned tablelength, 
    float* amplitudes,
    float* phases,
    unsigned paramslength
    ) {
  float inc = 0.f, angle = 0.f, numsamples = (float)tablelength;

  // Make sure amplitudes are within 0 & 1
  // scale(amplitudes, paramslength, 0.f, 1.f);
  // normalize(amplitudes, paramslength);

  // ------------ FORMULA --------------------
  // where n = 1 -> paramslength
  // inc = pi * 2.f * n / numsamples;
  // positionphase = inc + (numsamples * inc); 
  // value = positionphase * amplitude
  // -----------------------------------------
  
  for (int n=1; n <= paramslength; n++) {
    inc = TAU * n / numsamples;
    angle = inc * numsamples * phases[n-1];

    // magic extra length because of Buffer shenanigans
    for (int i=0; i<tablelength+1; i++) {
      table[i] += (sin_approx(angle, 10) * amplitudes[n-1]);
      angle += inc;
    }
  }

  // more shenanigans 
  scale(table, tablelength+1, -1.f, 1.f);
  // return for simplicity, the function mutates array in place through pointer.
  return table;
}
