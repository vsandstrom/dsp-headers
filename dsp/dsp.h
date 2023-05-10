#pragma once

#ifndef DSP_H
#define DSP_H
#endif

#include <cmath>

namespace dspheaders {

    inline float clamp(float x, float bot, float top) {
        return fmax(bot, (fmin(x, top)));
    }

    inline float scale(float x, float a, float  b) {
        return (b-a)*x + a;
    }

    inline float dcblock(float x, float xm1, float ym1) {
        return  x - xm1 + 0.995 * ym1;
    }

    inline float mtof(int n, float base = 440.f) {
        return base * pow(2,(n/12));
    }

} /* namespace dspheaders */
