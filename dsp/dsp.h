#pragma once

#ifndef DSP_H
#define DSP_H

#include <cmath>
namespace dspheaders {

  struct frame {
    float left;
    float right;
  };


  // Set hard min- and max amplitude limits on signal, where 'x' is signal
  inline float clamp(float x, float bot, float top) {
      return fmax(bot, (fmin(x, top)));
  }

  // Convert signal a range to new range, where 'x' is signal
  inline float map(float x, float inmin, float inmax, float outmin, float outmax) {
      return (outmax-outmin)*(x - inmin)/(inmax-inmin)+outmin;
  }

  // Highpass filter removing DC-offset
  //
  // x - current input sample: x[n]
  //
  // xm1 - previous input sample - FIR
  //
  // ym1 - previous output sample - IIR
  inline float dcblock(float x, float xm1, float ym1) {
      return  x - xm1 + 0.995 * ym1;
  }

  // Converts midinumber to frequency
  inline float mtof(int midinum, float base = 440.f) {
    return base * pow(2,(midinum/12));
  }

  // Converts from dB to linear volume
  inline float dBToVolume(float dB) {
    return powf(10.f, 0.05f * dB);
  }

  // Converts from linear volume to dB
  inline float volumeTodB(float volume) {
    return 20.f * log10f(volume);
  }

  // Mutate values in array with dspheaders::map, for each value
  inline void range(float* buffer, uint32_t bufferLength, float inmin, float inmax, float outmin, float outmax) {
    // Convert values in input buffer within input range to new range
    for (int i = 0; i < bufferLength; i++) {
      buffer[i] = map(buffer[i], inmin, inmax, outmin, outmax);
    }
  }
  
  // Transform value in range -1.0 - 1.0 to 0.0 - 1.0
  inline float tounipolar(float x) {return map(x, -1.f, 1.f, 0.f, 1.f);} 
   
  // Transform value in range 0.0 - 1.0 to -1.0 - 1.0
  inline float tobipolar(float x) {return map(x, 0.f, 1.f, -1.f, 1.f);}

  // Makes sure that x is within range of 0 - n 
  inline int wrap(int x, uint32_t length) {
    while (x < 0) x += length;
    while (x >= length) x -= length;
    return x;
  }

  // Makes sure that x is within range of 0.0 - n
  //
  // x is a kept as float for interpolation purposes.
  inline float wrapf(float x, uint32_t length) {
    float lengthf = (float)length;
    while (x < 0.f) x += lengthf;
    while (x >= lengthf) x -= lengthf;
    return x;
  }
} /* namespace dspheaders */

#endif
