#pragma once
#include <cmath>
#include <cstdlib>

namespace dspheaders {
  const float pi =            3.14159265358979323846264338327950288f;
  const float tau =           6.28318530717958647692528676655900577f;
  const float frac_pi_4 =     0.785398163397448309615660845819875721f;
  const float frac_1_sqrt_2 = 0.707106781186547524400844362104849039;

  struct frame { float left; float right; };

  enum GATE { on = 1, off = 0, cycle = 2, };

  ///////////////////////////////
  // Discrete sample manipulation
  ///////////////////////////////

  // Set hard min- and max amplitude limits on signal, where 'x' is signal
  inline float clamp(float x, float lo, float hi) {
      return fmax(lo, (fmin(x, hi)));
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
  
  inline float dcblock(float x, float xm1, float ym1) {
      return  x - xm1 + 0.995 * ym1;
  }

  /// INDEX WRAPPING FUNCTIONS - AVOID AND USE 
  // Makes sure that x is within range of 0 - n 
  inline int wrap(int* x, unsigned int length) {
    while (*x < 0) *x += length;
    while (*x >= length) *x -= length;
    return *x;
  }

#ifdef __ARM_ARCH

  inline unsigned wrap(unsigned* x, unsigned int length) {
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

inline unsigned wrap(unsigned* x, unsigned int length) {
  while (*x >= length) *x -= length;
  return *x;
}

#endif


  inline unsigned wrap_dangerously(unsigned int* x, unsigned int length) {
    // Should work for both positive and negative overflow of unsigned int
    // Since index should always be <= 0 this should work.
    *x &= (length -1);
    return *x;
  }

  // Makes sure that x is within range of 0.0 - n
  //
  // x is a kept as float for interpolation purposes.
  inline float wrapf(float* x, unsigned int length) {
    float lengthf = (float)length;
    while (*x < 0.f) *x += lengthf;
    while (*x >= lengthf) *x -= lengthf;
    return *x;
  }

  ////////////////////////////
  // Array/Buffer manipulation
  ////////////////////////////

  inline void initbuffer(float* buffer, unsigned bufferlength) {
    for (unsigned i = 0; i < bufferlength; ++i) *buffer++ = 0.f;
  }
  
  // Mutate values in array with dspheaders::map, for each value
  inline void range(
      float* buffer, unsigned int bufferLength,
      float inmin, float inmax,
      float outmin, float outmax) {
    // Convert values in input buffer within input range to new range
    for (int i = 0; i < bufferLength; i++) {
      buffer[i] = map(buffer[i], inmin, inmax, outmin, outmax);
    }
  }
  
  // Mutate
  inline void scale(float* buffer, unsigned length, float outmin, float outmax) {
    float min = 0.f, max = 0.f;
    for (int i=0; i<length; i++){
      if (buffer[i] < min) {min = buffer[i];}
      if (buffer[i] > max) {max = buffer[i];}
    }
    range(buffer, length, min, max, outmin, outmax);
  }   

  
  inline float sum(float* buffer, unsigned length) {
    float sum = 0.f;
    for (unsigned i = 0; i < length; i++) {
      sum += buffer[i];
    }
    return sum;
  }
  
  inline unsigned sum(unsigned* buffer, unsigned length) {
    float sum = 0.f;
    for (unsigned i = 0; i < length; i++) {
      sum += buffer[i];
    }
    return sum;
  }

  // Softmax function, useful when creating a wavetable, where total amplitude
  // should not excede -1 - 1
  inline float* normalize(float* buffer, unsigned length) {
    float factor = 1 / sum(buffer, length);
    int i;
    for (i=0; i<length; i++){
      buffer[i] *= factor;
    }
    return buffer;
  }
} /* namespace dspheaders */
