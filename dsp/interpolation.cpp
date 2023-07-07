#include <math.h>
#include "interpolation.hpp"
#include "./dsp.h"

using namespace dspheaders;
float Interpolation::linear(float position, float* table) {
  int a1 = position; // implicit cast
  int b1 = a1 + 1;
  float diff = position - a1;
  float bw = diff;
  float aw = 1 - diff;
  return table[a1] * aw + table[b1] * bw;
}

float Interpolation::cosine(float position, float *table) {
  int a1 = position; // implicit cast
  int b1 = a1 + 1;
  float diff = position - a1;
  float bw = (1 - cos(diff*pi)) / 2;
  float aw = 1.0 - bw;
  return table[a1] * aw + table[b1] * bw;
}

float Interpolation::cubic(float position, float* table, int tableLength) {
  // a - samples "behind", b - samples "ahead"
  int a1 = position; // implicit cast
  int b1 = a1 + 1;
  float diff = position - a1;
  float bw = diff;
  float aw = 1 - diff;
  // Since table is constructed with an n+1 size, we only need to
  // make sure samples read 2 steps removed is within bounds.
  int a2 = wrap(a1-1, tableLength);
  int b2 = wrap(b1+1, tableLength);
  float ax = table[a2] * aw + table[a1] * bw;
  float bx = table[b1] * aw + table[b2] * bw;
  return ax * aw + bx * bw;
}
