#pragma once

#include <cstddef>
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
      const float a = table[static_cast<size_t>(position)];
      const float b = table[static_cast<size_t>(position+1)];
      const float x = position - static_cast<long>(position);
      return a + x * (b - a);
    }

    // 2 point cosine interpolation
    inline float cosine(const float position, const float* const table, const size_t _tablelength){
      const float a = table[static_cast<size_t>(position)];
      const float b = table[static_cast<size_t>(position+1)];
      const float diff = position - static_cast<long>(position);
      const float x = (1 - cos(diff*PI)) * 0.5;
      return a + x * (b - a);
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
    inline float cubic(const float position, const float* const table, const size_t tablelength) {
      // positions
      const size_t a2 = position; // implicit cast
      const size_t b1 = a2+1;
      const size_t a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
      const size_t b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard

      const float diff = position - a2;

      // values
      const float x0 = table[a1];
      const float x1 = table[a2];
      const float x2 = table[b1];
      const float x3 = table[b2];

      // coefficients
      // float c0 = x3 - x2 - x0 + x1;
      // float c1 = x0 - x1 - c0;
      // float c2 = x2 - x0;
      // float c3 = x1;
      
      const float c2 = x2 - x0;
      const float c0 = x3 - c2 + x1;
      const float c1 = x0 - x1 - c0;

      return (c0 * pow(diff, 3)) + (c1 * (diff*diff)) + (c2 * diff) + x1;
    }

    // 4 point hermetic interpolation
    inline float hermetic(const float position, const float* const table, const size_t tablelength) {
      const size_t a2 = position;
      const size_t b1 = position + 1;
      const size_t a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
      const size_t b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard

      const float diff = position - a2;
      
      // values
      const float x0 = table[a1];
      const float x1 = table[a2];
      const float x2 = table[b1];
      const float x3 = table[b2];

      const float sub = x1 - x2;
      const float c1 = x2 - x0;
      const float c3 = x3 - x1 + 3 * sub;
      const float c2 = -(2 * sub + c1 + c3);
      return 0.5f * ((c3 * diff + c2) * diff + c1) * diff + x1;
    }

    inline float slope(const float a, const float b) {
      return (a + b) / 2;
    }

    inline float bilinear(const float a, const float b, const float c, const float d, const float x, const float y) {
      const float n = b - a;
      const float m = x * (n - c + d);
      return a + x * n + y * ( c - a + m );
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
          const float a = table[static_cast<size_t>(position)];
          const float b = table[static_cast<size_t>(position+1)];
          const float x = position - static_cast<long>(position);
          return a + x * (b - a);
        }
      };

      
      struct cosine : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          const float a = table[static_cast<size_t>(position)];
          const float b = table[static_cast<size_t>(position+1)];
          const float diff = position - static_cast<long>(position);
          const float x = (1 - cos(diff*PI)) * 0.5;
          return a + x * (b - a);
        }
      };
      
      struct cubic : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          // positions
          const size_t a2 = position; // implicit cast
          const size_t b1 = a2+1;
          const size_t a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
          const size_t b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard

          const float diff = position - a2;

          // values
          const float x0 = table[a1];
          const float x1 = table[a2];
          const float x2 = table[b1];
          const float x3 = table[b2];

          // coefficients
          // float c0 = x3 - x2 - x0 + x1;
          // float c1 = x0 - x1 - c0;
          // float c2 = x2 - x0;
          // float c3 = x1;
          
          const float c2 = x2 - x0;
          const float c0 = x3 - c2 + x1;
          const float c1 = x0 - x1 - c0;

          return (c0 * pow(diff, 3)) + (c1 * (diff*diff)) + (c2 * diff) + x1;
        }
      };
      
      struct hermetic : public Interpolation {
        float operator() (float position, float* table, size_t tablelength) const {
          const size_t a2 = position;
          const size_t b1 = position + 1;
          const size_t a1 = a2-1 < tablelength ? a2-1 : tablelength-1; // uint wrap-around guard
          const size_t b2 = a2+2 < tablelength ? a2+2 : 0;             // out-of-bounds guard

          const float diff = position - a2;
          
          // values
          const float x0 = table[a1];
          const float x1 = table[a2];
          const float x2 = table[b1];
          const float x3 = table[b2];

          const float sub = x1 - x2;
          const float c1 = x2 - x0;
          const float c3 = x3 - x1 + 3 * sub;
          const float c2 = -(2 * sub + c1 + c3);
          return 0.5f * ((c3 * diff + c2) * diff + c1) * diff + x1;
        }
      };
    }
  }
}

#endif
