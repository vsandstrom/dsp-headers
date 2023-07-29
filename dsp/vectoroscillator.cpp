#include "vectoroscillator.hpp"
#include <cstdio>
#include <vector>
using namespace dspheaders;
VectorOscillator::VectorOscillator(){}

VectorOscillator::VectorOscillator(
    std::vector<Wavetable> tables,
    float (*interpolate)(float, float*, unsigned))
  : tables(tables), interpolate(interpolate), tablelength(tables.size()) {
  weight = 0.0f;
  crossection = new float[tables.size()]();
}

float VectorOscillator::play(float transfer) {
  float weight = transfer * (float)tablelength -1;

  for (int i = 0; i < tablelength; ++i) {
    tables[i].frequency = frequency;
    crossection[i] = tables[i].play();
  }

  return interpolate(weight, crossection, tablelength);
}

float VectorOscillator::play(float transfer, float phase) {
  float weight = transfer * (float)tablelength -1;

  for (int i = 0; i < tablelength; ++i) {
    tables[i].frequency = frequency;
    crossection[i] = tables[i].play(phase);
  }

  return interpolate(weight, crossection, tablelength);

}
