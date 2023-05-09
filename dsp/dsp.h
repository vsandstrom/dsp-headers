#pragma once

#ifndef DSP_H
#define DSP_H
#endif

#include <cmath>

inline float clamp(float x, float bot, float top) {
    return fmax(bot, (fmin(x, top)));
}

inline float scale(float x, float y, float a, b) {
    return a*b*x*y;
}

inline float dcblock(float x, float xm1, float ym1) {
    float y = x - xm1 + 0.995 * ym1;
    xm1 = x;
    ym1 = y;
    return y;
}
