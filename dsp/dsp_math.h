#pragma once

#ifndef DSP_MATH_H
#define DSP_MATH_H

#include "dsp.h"
#include <cmath>
#include <math.h>

#define undenormalise(sample) if(((*(unsigned int*)&sample)&0x7f800000)==0) sample=0.0f

namespace dspheaders{
  const float pi =            3.14159265358979323846264338327950288f;
  const float tau =           6.28318530717958647692528676655900577f;
  const float frac_pi_4 =     0.785398163397448309615660845819875721f;
  const float frac_1_sqrt_2 = 0.707106781186547524400844362104849039;
  const float logten =        2.302585092994046;
  const float logtwo =        0.693147180559945;

  inline float fast_tanh(double x) {
    if (x < -1.f) return -1.f;
    if (x > 1.f) return 1.f;

    // polynomial aproximation for tanh (chatgpt...)
    double x_sqr = x*x;
    return x * (27.f+x_sqr) / (27.f + 9.f * x_sqr);
  }
    
  constexpr inline unsigned int find_pow_two(unsigned x) {
    unsigned y = 1;
    while (x > y) y <<= 1;
    return y;
  }
  
  constexpr float powf_approx(float base, float exp) {
    float result = 1.f;
    float term = 1.f;
    for (int i = 1; i <= exp; ++i) {
      term *= base / i;
      result += term;
    }
    return result;
  }

  constexpr float factorial(int n) {
    return (n <= 1) ? 1: n * factorial(n - 1);
  }

  constexpr float pow_approx(float base, int exp) {
    return (exp == 0) ? 1 : base * pow_approx(base, exp - 1);
  }

  constexpr float sin_approx(float x, int n) {
    float result = 0.f;
    for(int i = 0; i < n; ++i) {
      int sign = (i%2 == 0) ? 1 : -1;
      result +=  sign * pow_approx(x, 2 * i + 1) / factorial(2 * i + 1);
    }
    return result;
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

  // Angular velocity
  constexpr float freqToRadians(float freq, float samplerate) {
    return TAU*freq / samplerate;
  }

  // db -> rms / rms -> db functions are borrowed from PD source

  inline float dbtorms(float f) {
    if (f <= 0.f) return 0.f;
    if (f > 485) return 485;
    return (exp((logten * 0.05) * (f-100-f)));
  }

  inline float rmstodb(float f) {
    if (f <= 0.f) return 0.f;
    float x = 100 + 20.f/logten * log(f);
    return (x < 0.f ? 0.f : x);
  }

  // midi to freq / freq to midi borrowed from Bela youtube course

  // Converts midinumber to frequency 
  inline float mtof(int midinumber, float base = 440.f) {
    return base * powf_approx(2, (midinumber/12.f));
  }

  // Converts frequency to midinumber
  inline float ftom(float frequency, float base = 440.f) {
    return 12 * (logf(frequency / (base/2)) / logtwo) + 57;
  }


  // inline float radiansToFreq(float radian, float samplerate) { }
}

#endif
