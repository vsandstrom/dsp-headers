#pragma once

#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include <cstdlib>
#include <cmath>
#include "dsp_math.h"

namespace dspheaders {
  namespace interpolation {
    // In case there should be no interpolation at all
    inline float none(const float position, const float* const table, const size_t _tablelength) {
      return table[(int)position];
    };

  /* |-----------------|
   * |    2 sample     |
   * |  interpolation  |
   * |--[a1]--|--[b1]--|
   * |        |        |
   * |   n-1  |  n+1   |
   * |        |        |
   * |     [ pos ]     |
   * |--------|--------|
   */ 


    // Basic 2 point linear interpolation
    inline float linear(const float position, const float* const table, const size_t tablelength) {
      int a1, b1 = 0;
      float w, diff = 0.f;

      a1 = position; // implicit cast
      b1 = a1 + 1;
      diff = position - a1;
      w = diff;
      return table[a1] + w * (table[b1] - table[a1]);
    }

    // 2 point cosine interpolation
    inline float cosine(const float position, const float* const table, const size_t _tablelength){
      int a1, b1 = 0;
      float aw, bw, diff = 0.f;

      a1 = position; // implicit cast
      b1 = a1 + 1;
      diff = position - a1;
      bw = (1 - cos(diff*PI)) / 2;
      aw = 1.0 - bw;
      return table[a1] * aw + table[b1] * bw;
    }
  
    /* |-----------------------------------|
     * |     4 sample interpolation        |
     * |--[a1]--|--[a2]--|--[b1]--|--[b2]--|
     * |        |        |        |        |
     * |  n-1   |   n    |  n+1   |  n+2   |
     * |        |        |        |        |
     * |        |     [ pos ]     |        |
     * |-----------------------------------|
     */

    // 4 point cubic interpolation
    inline float cubic(const float position, const float* const table, const size_t tableLength) {
      size_t a1, a2, b1, b2;
      float c0, c1, c2, diff;

      // positions
      a2 = position; // implicit cast
      b1 = a2+1;
      a1 = a2-1 < tableLength ? a2-1 : tableLength-1; // uint wrap-around guard
      b2 = a2+2 < tableLength ? a2+2 : 0;             // out-of-bounds guard

      diff = position - a2;

      // values
      // float x0 = table[a1];
      // float x1 = table[a2];
      // float x2 = table[b1];
      // float x3 = table[b2];

      // coefficients
      // float c0 = x3 - x2 - x0 + x1;
      // float c1 = x0 - x1 - c0;
      // float c2 = x2 - x0;
      // float c3 = x1;
      
      c0 = table[b2] - table[b1] - table[a1] + table[a2];
      c1 = table[a1] - table[a2] - c0;
      c2 = table[b1] - table[a1];

      return (c0 * pow(diff, 3)) + (c1 * (diff*diff)) + (c2 * diff) + table[a2];
    }

    // 4 point hermetic interpolation
    inline float hermetic(const float position, const float* const table, const size_t tableLength) {
      if (table==NULL) return 0.f;
      size_t a1, a2, b1, b2;
      float c1, c2, c3, sub, diff;
      a2 = position;
      b1 = position + 1;
      a1 = a2-1 < tableLength ? a2-1 : tableLength-1; // uint wrap-around guard
      b2 = a2+2 < tableLength ? a2+2 : 0;             // out-of-bounds guard

      diff = position - a2;

      sub = table[a2] - table[b1];
      c1 = table[b1] - table[a1];
      c3 = table[b2] - table[a2] + 3 * sub;
      c2 = -(2 * sub + c1 + c3);
      return 0.5f * ((c3 * diff + c2) * diff + c1) * diff + table[a2];
    }

    inline float slope(const float a, const float b) {
      return (a + b) / 2;
    }

    inline float bilinear(const float a, const float b, const float c, const float d, const float x, const float y) {
      const float n = b - a;
      const float m = x * (n + (c-d));
      return a + x * n + y * (n + x * m);
    }
    
    namespace functors {
      struct Interpolation {
        virtual float operator()(float position, float* table, size_t tablelength);
      };

      struct floor : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          return table[(int)position];
        }
      };

      struct linear : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          int a1, b1 = 0;
          float aw, bw, diff = 0.f;

          a1 = position; // implicit cast
          b1 = a1 + 1;
          diff = position - a1;
          bw = diff;
          aw = 1 - diff;
          return table[a1] * aw + table[b1] * bw;
        }
      };

      
      struct cosine : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          int a1, b1 = 0;
          float aw, bw, diff = 0.f;

          a1 = position; // implicit cast
          b1 = a1 + 1;
          diff = position - a1;
          bw = (1 - cos(diff*PI)) / 2;
          aw = 1.0 - bw;
          return table[a1] * aw + table[b1] * bw;

        }
      };
      
      struct cubic : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          size_t a1, a2, b1, b2;
          float c0, c1, c2, diff;

          // positions
          a2 = position; // implicit cast
          b1 = a2+1;
          a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
          b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard

          diff = position - a2;

          // values
          // float x0 = table[a1];
          // float x1 = table[a2];
          // float x2 = table[b1];
          // float x3 = table[b2];

          // coefficients
          // float c0 = x3 - x2 - x0 + x1;
          // float c1 = x0 - x1 - c0;
          // float c2 = x2 - x0;
          // float c3 = x1;
          
          c0 = table[b2] - table[b1] - table[a1] + table[a2];
          c1 = table[a1] - table[a2] - c0;
          c2 = table[b1] - table[a1];

          return (c0 * pow(diff, 3)) + (c1 * (diff*diff)) + (c2 * diff) + table[a2];

        }
      };
      
      struct hermetic : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          if (table==NULL) return 0.f;
          size_t a1, a2, b1, b2;
          float c1, c2, c3, sub, diff;
          a2 = position;
          b1 = position + 1;
          a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
          b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard

          diff = position - a2;

          sub = table[a2] - table[b1];
          c1 = table[b1] - table[a1];
          c3 = table[b2] - table[a2] + 3 * sub;
          c2 = -(2 * sub + c1 + c3);
          return 0.5f * ((c3 * diff + c2) * diff + c1) * diff + table[a2];
        }
      };
    }
  }
}

#endif
