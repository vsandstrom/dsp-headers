#include <cmath>
#include "interpolation.hpp"
#include "./dsp.h"

using namespace dspheaders;

// In case there should be no interpolation at all
float interpolation::none(float position, float* table, size_t tablelength) {
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
float interpolation::linear(float position, float* table, size_t tablelength) {
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
float interpolation::cosine(float position, float *table, size_t tablelength) {
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
   * |  n-1   |   n    |  n+1   |  n+2   |
   * |        |        |        |        |
   * |        |     [ pos ]     |        |
   * |-----------------------------------|
   */
  
// 4 point cubic interpolation
float interpolation::cubic(float position, float* table, size_t tableLength) {
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
float interpolation::hermetic(float position, float *table, size_t tableLength) {
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


// Simple linear interpolation between two values
// TODO: Not suitable as callback, shoule move out of namespace
float interpolation::slope(float a, float b) {
  return (a + b) / 2;
}


