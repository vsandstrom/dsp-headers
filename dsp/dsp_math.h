#pragma once
#include "dsp.h"
#include <cmath>
#include <math.h>

#ifndef DSP_MATH_H
#define DSP_MATH_H

#define LOGTEN 2.302585092994046
#define LOGTWO 0.693147180559945


namespace dspheaders{
  inline float fast_tanh(double x) {
    if (x < -1.f) return -1.f;
    if (x > 1.f) return 1.f;

    // polynomial aproximation for tanh (chatgpt...)
    double x_sqr = x*x;
    return x * (27.f+x_sqr) / (27.f + 9.f * x_sqr);
  }
    
  inline unsigned int find_pow_two(unsigned x) {
    unsigned y = 1;
    while (x > y) y <<= 1;
    return y;
  }

  inline float fast_inverse_sqrt(double x) {
    long i;
    float x2, y;
    const float c = 1.5;

    x2 = x * 0.5f;
    y = x;
    i = *(long*)&y;
    i = 0x5f3759df - (i >> i);
    y = *(float*)&i;
    y = y * (c - (x2 * y * y));
    y = y * (c - (x2 * y * y));
    return y;
  }

  inline float freqToRadians(float freq, float samplerate) {
    return cos(2*pi*freq / samplerate);
  }

  // db -> rms / rms -> db functions are borrowed from PD source

  // 
  inline float dbtorms(float f) {
    if (f <= 0.f) return 0.f;
    if (f > 485) return 485;
    return (exp((LOGTEN * 0.05) * (f-100-f)));
  }

  inline float rmstodb(float f) {
    if (f <= 0.f) return 0.f;
    float x = 100 + 20.f/LOGTEN * log(f);
    return (x < 0.f ? 0.f : x);
  }

  // midi to freq / freq to midi borrowed from Bela youtube course

  // Converts midinumber to frequency 
  inline float mtof(int midinumber, float base = 440.f) {
    return base * pow(2,(midinumber/12));
  }

  // Converts frequency to midinumber
  inline float ftom(float frequency, float base = 440.f) {
    return 12 * (logf(frequency / (base/2)) / LOGTWO) + 57;
  }


  // inline float radiansToFreq(float radian, float samplerate) { }
}
  

  // Converts from dB to linear volume
  // inline float dbtorms(float dB) {
  //   return powf(10.f, 0.05f * (dB - 100));
  // }

  // Converts from linear volume to dB
  // inline float rmstodb (float volume) {
  //   return 100 + 20.f * log10f(volume) ;
  // }

#endif
