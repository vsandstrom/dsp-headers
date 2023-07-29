#include <math.h>
#include "interpolation.hpp"
#include "./dsp.h"

using namespace dspheaders;

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
float interpolation::linear(float position, float* table, unsigned tablelength) {
  int a1, b1 = 0;
  float aw, bw, diff = 0.f;

  a1 = position; // implicit cast
  b1 = a1 + 1;
  diff = position - a1;
  bw = diff;
  aw = 1 - diff;
  return table[a1] * aw + table[b1] * bw;
}

// 2 point cosine interpolation
float interpolation::cosine(float position, float *table, unsigned tablelength) {
  int a1, b1 = 0;
  float aw, bw, diff = 0.f;

  a1 = position; // implicit cast
  b1 = a1 + 1;
  diff = position - a1;
  bw = (1 - cos(diff*pi)) / 2;
  aw = 1.0 - bw;
  return table[a1] * aw + table[b1] * bw;
}

  /* |-----------------------------------|
   * |     4 sample interpolation        |
   * |--[a1]--|--[a2]--|--[b1]--|--[b2]--|
   * |        |        |        |        |
   * |  n-2   |   n-1  |  n+1   |  n+2   |
   * |        |        |        |        |
   * |        |     [ pos ]     |        |
   * |-----------------------------------|
   */
  
// 4 point linear interpolation
// float Interpolation::bilinear(float position, float* table, int tableLength) {
//   int a1, a2, b1, b2 = 0;
//   float aw, bw, diff, ax, bx = 0.f; 
//   // a - samples "behind", b - samples "ahead"
//   a2 = position; // implicit cast
//   b1 = a2 + 1;
//   diff = position - a2;
//   bw = diff;
//   aw = 1 - diff;
//   // Since table is constructed with an n+1 size, we only need to
//   // make sure samples read 2 steps removed is within bounds.
//   a1 = wrap(a2-1, tableLength);
//   b2 = wrap(b1+1, tableLength);
//   ax = table[a1] * aw + table[a2] * bw;
//   bx = table[b1] * aw + table[b2] * bw;
//   return ax * aw + bx * bw;
// }

// 4 point cubic interpolation
float interpolation::cubic(float position, float* table, unsigned tableLength) {
  int a1, a2, b1, b2;
  float c0, c1, c2, diff;

  // positions
  a2 = position;
  b1 = a2+1;
  // Since table is constructed with an n+1 size, we only need to
  // make sure samples read 2 steps removed is within bounds.
  a1 = wrap(a2-1, tableLength);
  b2 = wrap(b1+1, tableLength);

  diff = position - a2;
  // values
  // float x0 = table[a1];
  // float x1 = table[a2];
  // float x2 = table[b1];
  // float x3 = table[b2];

  c0 = table[b2] - table[b1] - table[a1] + table[a2];
  c1 = table[a1] - table[a2] - c0;
  c2 = table[b1] - table[a1];

  // coefficients
  // float c0 = x3 - x2 - x0 + x1;
  // float c1 = x0 - x1 - c0;
  // float c2 = x2 - x0;
  // float c3 = x1;
  return (c0 * pow(diff, 3)) + (c1 * (diff*diff)) + (c2 * diff) + table[a2];
}

// 4 point hermetic interpolation
float interpolation::hermetic(float position, float *table, unsigned tableLength) {
  if (table==NULL) return 0.f;
  int a1, a2, b1, b2;
  float c1, c2, c3, sub, diff;
  a2 = position;
  b1 = position + 1;
  a1 = wrap(a2-1, tableLength);
  b2 = wrap(b1+1, tableLength);

  diff = position - a2;

  sub = table[a2] - table[b1];
  c1 = table[b1] - table[a1];
  c3 = table[b2] - table[a2] + 3 * sub;
  c2 = -(2 * sub + c1 + c3);
  return 0.5f * ((c3 * diff + c2) * diff + c1) * diff + table[a2];
}

// In case there should be no interpolation at all
float interpolation::none(float position, float* table, unsigned tablelength) {
  return table[(int)position];
};

