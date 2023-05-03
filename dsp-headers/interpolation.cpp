#include <math.h>
#include "interpolation.hpp"

float Interpolation::linear(float position, float* table){
  int prevPosition = position; // implicit cast
  int nextPosition = prevPosition + 1;
  float diff = position - (float)prevPosition;
  float nextWeight = diff;
  float prevWeight = 1 - diff;

  return table[prevPosition] * prevWeight + table[nextPosition] * nextWeight;
}

float Interpolation::cosine(float position, float *table) {
  int prevPosition = position; // implicit cast
  int nextPosition = prevPosition + 1;
  float diff = position - floor(position);
  float nextWeight = (1 - cos(diff*pi)) / 2;
  float prevWeight = 1.0 - nextWeight;
  return table[prevPosition] * prevWeight + table[nextPosition] * nextWeight;
}
