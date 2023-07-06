#pragma once

#ifndef DSP_H
#define DSP_H

#include <cmath>
namespace dspheaders {

  struct frame {
    float left;
    float right;
  };


  inline float clamp(float x, float bot, float top) {
    // Set hard min- and max amplitude limits on signal
      return fmax(bot, (fmin(x, top)));
  }

  inline float map(float x, float inmin, float inmax, float outmin, float outmax) {
    // Convert input value within input range to new range
      return (outmax-outmin)*(x - inmin)/(inmax-inmin)+outmin;
  }

  inline float dcblock(float x, float xm1, float ym1) {
    // x - current input sample: x[n]
    // xm1 - previous input sample - FIR
    // ym1 - previous output sample - IIR
      return  x - xm1 + 0.995 * ym1;
  }

  inline float mtof(int midinum, float base = 440.f) {
    // Converts midinumber to frequency
    return base * pow(2,(midinum/12));
  }

  inline float dBToVolume(float dB) {
    // 
    return powf(10.f, 0.05f * dB);
  }

  inline float volumeTodB(float volume) {
    return 20.f * log10f(volume);
  }

  inline void range(float* buffer, uint32_t bufferLength, float inmin, float inmax, float outmin, float outmax) {
    // Convert values in input buffer within input range to new range
    for (int i = 0; i < bufferLength; i++) {
      buffer[i] = map(buffer[i], inmin, inmax, outmin, outmax);
    }
  }
  
  // Normalize a value to a range of 0.0 - 1.0
  // Useful when using a bipolar LFO to modulate phase in an oscillator. 
  inline void unipolar(float sample) {
    map(sample, -1.f, 1.f, 0.f, 1.f);
  } 

  inline int wrap(int x, uint32_t len) {
    while (x < 0) x += len;
    while (x >= len) x -= len;
    return x;
  }
    
} /* namespace dspheaders */

#endif
