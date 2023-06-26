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
      return fmax(bot, (fmin(x, top)));
  }

  inline float map(float x, float inmin, float inmax, float outmin, float outmax) {
      return (outmax-outmin)*(x - inmin)/(inmax-inmin)+outmin;
  }

  inline float dcblock(float x, float xm1, float ym1) {
      return  x - xm1 + 0.995 * ym1;
  }

  inline float mtof(int n, float base = 440.f) {
      return base * pow(2,(n/12));
  }

  inline float dBToVolume(float dB) {
    return powf(10.f, 0.05f * dB);
  }

  inline float volumeTodB(float volume) {
    return 20.f * log10f(volume);
  }

  inline void range(float* buffer, uint32_t bufferLength, float inmin, float inmax, float outmin, float outmax) {
    for (int i = 0; i < bufferLength; i++) {
      buffer[i] = map(buffer[i], inmin, inmax, outmin, outmax);
    }
  }
  
  // Normalize a value to a range of 0.0 - 1.0
  // Useful when using a bipolar LFO to modulate phase in an oscillator. 
  inline void unipolar(float sample) {
    map(sample, -1.f, 1.f, 0.f, 1.f);
  } 
    
} /* namespace dspheaders */

#endif
