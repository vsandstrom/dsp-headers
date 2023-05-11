#pragma once

#ifndef DSP_H
#define DSP_H
#endif

#include <cmath>

namespace dspheaders {

    inline float clamp(float x, float bot, float top) {
        return fmax(bot, (fmin(x, top)));
    }

    inline float scale(float x, float imin, float imax, float omin, float  omax) {
      return (omax - omin) * (x - imin)/(imax - imin) + omin;
    }

    void range(float * x, int xLen, float imin, float imax, float omin, float omax) {
        for (int i = 0; i < xLen; ++i) {
            x[i] = scale(x[i], imin, imax, omin, omax); 
        }
    }

    inline float dcblock(float x, float xm1, float ym1) {
        return  x - xm1 + 0.995 * ym1;
    }

    inline float mtof(int n, float base = 440.f) {
        return base * pow(2,(n/12));
    }

} /* namespace dspheaders */
