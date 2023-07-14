#include <math.h>
#include "interpolation.hpp"
#include "./dsp.h"

using namespace dspheaders;
float Interpolation::linear(float position, float* table) {
  if (table==NULL) return 0.f;
  int a1 = position; // implicit cast
  int b1 = a1 + 1;
  float diff = position - a1;
  float bw = diff;
  float aw = 1 - diff;
  return table[a1] * aw + table[b1] * bw;
}

float Interpolation::cosine(float position, float *table) {
  if (table==NULL) return 0.f;
  int a1 = position; // implicit cast
  int b1 = a1 + 1;
  float diff = position - a1;
  float bw = (1 - cos(diff*pi)) / 2;
  float aw = 1.0 - bw;
  return table[a1] * aw + table[b1] * bw;
}

float Interpolation::bilinear(float position, float* table, int tableLength) {
  if (table==NULL) return 0.f;
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

float Interpolation::cubic(float position, float* table, int tableLength) {
  // positions
  int a1 = position;
  int b1 = a1+1;
  int a2 = wrap(a1-1, tableLength);
  int b2 = wrap(b1+1, tableLength);

  float diff = position - a1;
  // values
  float x0 = table[a1];
  float x1 = table[a2];
  float x2 = table[b1];
  float x3 = table[b2];
  // coefficients
  float c0 = x3 - x2 - x0 + x1;
  float c1 = x0 - x1 - c0;
  float c2 = x2 - x0;
  float c3 = x1;
  return (c0 * pow(diff, 3)) + (c1 * (diff*diff)) + (c2 * diff) + c3;
}

float Interpolation::hermetic(float position, float *table, int tableLength) {
  int a1 = position;
  int b1 = position + 1;
  int a2 = wrap(a1-1, tableLength);
  int b2 = wrap(b1+1, tableLength);

  float diff = position - a1;

  float sub = table[a1] - table[b1];
  float c1 = table[b1] - table[a2];
  float c3 = table[b2] - table[a1] + 3 * sub;
  float c2 = -(2 * sub + c1 + c3);
  return 0.5f * ((c3 * diff + c2) * diff + c1) * diff + table[a1];

}

