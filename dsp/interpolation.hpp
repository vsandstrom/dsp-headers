#include <cstdlib>
#include <cmath>
#include <functional>
#include "./dsp.h"

#pragma once

namespace dspheaders {
  namespace interpolation {
    float none(float position, float* table, size_t tablelength);
    float linear(float position, float* table, size_t tablelength);
    float cosine(float position, float* table, size_t tablelength);
    float cubic(float position, float* table, size_t tableLength);
    float hermetic(float position, float* table, size_t tableLength);
    float slope(float a, float b);

    // Below is a rejected implementation of the above functions as a functor
    // with an abstract interface class. this does not work

    // // Interface
    // class Interpolation {
    //   virtual float operator()(float position, float* table, size_t tablelength);
    // };
    //
    // // Derived classes implementing Interpolation interface
    // class floor : public Interpolation {
    //   float operator() (float position, float* table, size_t tablelength) const {
    //     return table[(int)position];
    //   }
    // };
    //
    // class linear : public Interpolation {
    //   float operator() (float position, float* table, size_t tablelength) const {
    //     int a1, b1 = 0;
    //     float aw, bw, diff = 0.f;
    //
    //     a1 = position; // implicit cast
    //     b1 = a1 + 1;
    //     diff = position - a1;
    //     bw = diff;
    //     aw = 1 - diff;
    //     return table[a1] * aw + table[b1] * bw;
    //   }
    // };
    //
    // 
    // class cosine : public Interpolation {
    //   float operator() (float position, float* table, size_t tablelength) const {
    //     int a1, b1 = 0;
    //     float aw, bw, diff = 0.f;
    //
    //     a1 = position; // implicit cast
    //     b1 = a1 + 1;
    //     diff = position - a1;
    //     bw = (1 - cos(diff*pi)) / 2;
    //     aw = 1.0 - bw;
    //     return table[a1] * aw + table[b1] * bw;
    //
    //   }
    // };
    // 
    // class cubic : public Interpolation {
    //   float operator() (float position, float* table, size_t tablelength) const {
    //     size_t a1, a2, b1, b2;
    //     float c0, c1, c2, diff;
    //
    //     // positions
    //     a2 = position; // implicit cast
    //     b1 = a2+1;
    //     a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
    //     b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard
    //
    //     diff = position - a2;
    //
    //     // values
    //     // float x0 = table[a1];
    //     // float x1 = table[a2];
    //     // float x2 = table[b1];
    //     // float x3 = table[b2];
    //
    //     // coefficients
    //     // float c0 = x3 - x2 - x0 + x1;
    //     // float c1 = x0 - x1 - c0;
    //     // float c2 = x2 - x0;
    //     // float c3 = x1;
    //     
    //     c0 = table[b2] - table[b1] - table[a1] + table[a2];
    //     c1 = table[a1] - table[a2] - c0;
    //     c2 = table[b1] - table[a1];
    //
    //     return (c0 * pow(diff, 3)) + (c1 * (diff*diff)) + (c2 * diff) + table[a2];
    //
    //   }
    // };
    // 
    // class hermetic : public Interpolation {
    //   float operator() (float position, float* table, size_t tablelength) const {
    //     if (table==NULL) return 0.f;
    //     size_t a1, a2, b1, b2;
    //     float c1, c2, c3, sub, diff;
    //     a2 = position;
    //     b1 = position + 1;
    //     a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
    //     b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard
    //
    //     diff = position - a2;
    //
    //     sub = table[a2] - table[b1];
    //     c1 = table[b1] - table[a1];
    //     c3 = table[b2] - table[a2] + 3 * sub;
    //     c2 = -(2 * sub + c1 + c3);
    //     return 0.5f * ((c3 * diff + c2) * diff + c1) * diff + table[a2];
    //   }
    // };

  }
}


