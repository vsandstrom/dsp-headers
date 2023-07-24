#include "./buffer.hpp"
#include "dsp.h"

using namespace dspheaders;

float BufferL::readSample(float readptr) {
  return Interpolation::linear(wrapf(readptr, bufferlength), buffer);
}

// Faulty - does sometimes give strange values
// float BufferL2::readSample(float readptr) {
//   return Interpolation::bilinear(wrapf(readptr, bufferlength), buffer, bufferlength);
// }

float BufferC::readSample(float readptr) {
  return Interpolation::cubic(wrapf(readptr, bufferlength), buffer, bufferlength);
}

float BufferH::readSample(float readptr) {
  return Interpolation::hermetic(wrapf(readptr, bufferlength), buffer, bufferlength);
}

