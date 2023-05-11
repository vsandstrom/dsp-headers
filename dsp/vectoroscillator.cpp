#include "vectoroscillator.hpp"
#include <cstdio>
#include <vector>

namespace dspheaders {

VectorOscillator::VectorOscillator(int sampleRate) : sampleRate(sampleRate) {}

VectorOscillator::VectorOscillator(
                                  std::vector<WaveTable> tables,
                                  int sampleRate,
                                  INTERPOLATION transferInterpolation
                                  ): 
                                tables(tables),
                                sampleRate(sampleRate),
                                transferInterpolation(transferInterpolation)
{
  tableWidth = tables.size() -1;
  weight = 0.0f;
  crossection = new float[tables.size()]();
}

float VectorOscillator::play(float transfer) {
  float weight = transfer* (float)tableWidth;

  for (int i = 0; i < tables.size(); ++i) {
    tables[i].frequency = frequency;
    crossection[i] = tables[i].play();
  }

  return Interpolation::linear(weight, crossection);
}

float VectorOscillator::play(float phase, float transfer) {
  float weight = transfer * (float)tableWidth;

  for (int i = 0; i < tables.size(); ++i) {
    tables[i].frequency = frequency;
    crossection[i] = tables[i].play(phase);
  }

  return Interpolation::linear(weight, crossection);
}

}
