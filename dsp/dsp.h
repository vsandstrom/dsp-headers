#pragma once

#include <cstddef>
#ifndef DSP_H
#define DSP_H

#include <cmath>
#include <cstdlib>

namespace dspheaders {
  struct frame { float left; float right; };

  enum GATE { on = 1, off = 0, cycle = 2, };

  ///////////////////////////////
  // Discrete sample manipulation
  ///////////////////////////////
  
  inline float fmax(float a, float b) {
    float r;
#ifdef __arm__
    asm("vmaxnm.f32 %[d], %[n], %[m]" : [d] "=t"(r) : [n] "t"(a), [m] "t"(b) :);
#else
    r = (a > b) ? a : b;
#endif // __arm__
    return r;
  }

  inline float fmin(float a, float b) {
    float r;
#ifdef __arm__
    asm("vminnm.f32 %[d], %[n], %[m]" : [d] "=t"(r) : [n] "t"(a), [m] "t"(b) :);
#else
    r = (a < b) ? a : b;
#endif // __arm__
    return r;
  }

  // Set hard min- and max amplitude limits on signal, where 'x' is signal
  inline float clamp(float in, float min, float max) {
      return fmin(fmax(in, min), max);
  }

  // Convert signal a range to new range, where 'x' is signal
  inline float map(float x, float inmin, float inmax, float outmin, float outmax) {
      return (outmax-outmin)*(x - inmin)/(inmax-inmin)+outmin;
  }
  
  // Transform value in range -1.0 - 1.0 to 0.0 - 1.0
  inline float tounipolar(float x) {return map(x, -1.f, 1.f, 0.f, 1.f);} 
   
  // Transform value in range 0.0 - 1.0 to -1.0 - 1.0
  inline float tobipolar(float x) {return map(x, 0.f, 1.f, -1.f, 1.f);}

  inline float fold(float x, float max) {
    if (x > max) return x - (x-max);
    if (x < -max) return x + (max + x);
    return x;
  }

  // https://www.dsprelated.com/freebooks/filters/DC_Blocker.html 
  // x = input
  // xm1 = previous input
  // ym1 = previous output
  inline float dcblock(float x, float xm1, float ym1) {
      return  x - xm1 + 0.995 * ym1;
  }

  /// INDEX WRAPPING FUNCTIONS - AVOID AND USE 
  // Makes sure that x is within range of 0 - n 
  inline int wrap(int* x, size_t length) {
    while (*x < 0) *x += length;
    while ((size_t)*x >= length) *x -= length;
    return *x;
  }

#ifdef __ARM_ARCH

  inline unsigned wrap(unsigned* x, size_t length) {
    int ret;
    asm ( \
        "mov %[w]. %[c] " \
        "subs %[w], %[w], %[s]\n" \
        "ands %[w], %[w], %[m]\n" \
        : [w] "=r" (ret) \
        : [c] "r" (x), [s] "r" (length), [m] "r" (length-1) \
        : "cc" \
    );
    return *x;
  }

#else

inline unsigned wrap(unsigned* x, size_t length) {
  while (*x >= length) *x -= length;
  return *x;
}

#endif


  inline unsigned wrap_dangerously(unsigned int* x, size_t length) {
    // Should work for both positive and negative overflow of unsigned int
    // Since index should always be <= 0 this should work.
    *x &= (length -1);
    return *x;
  }

  // Makes sure that x is within range of 0.0 - n
  //
  // x is a kept as float for interpolation purposes.
  inline float wrapf(float* x, size_t length) {
    float lengthf = (float)length;
    while (*x < 0.f) *x += lengthf;
    while (*x >= lengthf) *x -= lengthf;
    return *x;
  }

  ////////////////////////////
  // Array/Buffer manipulation
  ////////////////////////////

  inline void initbuffer(float* buffer, size_t bufferlength) {
    for (size_t i = 0; i < bufferlength; ++i) *buffer++ = 0.f;
  }
  
  // Mutate values in array with dspheaders::map, for each value
  inline void range(
      float* buffer, size_t bufferLength,
      float inmin, float inmax,
      float outmin, float outmax) {
    // Convert values in input buffer within input range to new range
    for (size_t i = 0; i < bufferLength; i++) {
      buffer[i] = map(buffer[i], inmin, inmax, outmin, outmax);
    }
  }
  
  // Mutate
  inline void scale(float* buffer, size_t length, float outmin, float outmax) {
    float min = 0.f, max = 0.f;
    for (size_t i=0; i<length; i++){
      if (buffer[i] < min) {min = buffer[i];}
      if (buffer[i] > max) {max = buffer[i];}
    }
    range(buffer, length, min, max, outmin, outmax);
  }   

  
  inline float sum(float* buffer, size_t length) {
    float sum = 0.f;
    for (size_t i = 0; i < length; i++) {
      sum += buffer[i];
    }
    return sum;
  }

  
  inline unsigned sum(unsigned* buffer, size_t length) {
    float sum = 0.f;
    for (size_t i = 0; i < length; i++) {
      sum += buffer[i];
    }
    return sum;
  }

  // Softmax function, useful when creating a wavetable, where total amplitude
  // should not excede -1 - 1
  inline float* normalize(float* buffer, size_t length) {
    float factor = 1 / sum(buffer, length);
    for (size_t i=0; i<length; i++){
      buffer[i] *= factor;
    }
    return buffer;
  }
} /* namespace dspheaders */

#endif
