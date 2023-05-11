#pragma once

#ifndef DSP_H
#define DSP_H
#endif

#include <cmath>



namespace dspheaders {
  // struct MinMax {
  //   float minimum;
  //   float maximum;
  // }

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

    inline void range(float* x, int xLen, float inmin, float inmax, float outmin, float outmax) {
      for (int i = 0; i < xLen; i++) {
        x[i] = map(x[i], inmin, inmax, outmin, outmax);
      }
    }
    
    // Normalize values to a range of 0.0 - 1.0
    // Useful when using a bipolar LFO to modulate phase in an oscillator. 
    inline void unipolar(float* x, int xLen) {
      range(x, xLen, -1.f, 1.f, 0.f, 1.f);
    } 
    
} /* namespace dspheaders */
