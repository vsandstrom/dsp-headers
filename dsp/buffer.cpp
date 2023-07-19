#include "./buffer.hpp"
#include "dsp.h"

using namespace dspheaders;

float BufferL::readSample(float readptr) {
  return Interpolation::linear(wrapf(readptr, bufferLength), buffer);
}

// Faulty - does sometimes give strange values
// float BufferL2::readSample(float readptr) {
//   return Interpolation::bilinear(wrapf(readptr, bufferLength), buffer, bufferLength);
// }

float BufferC::readSample(float readptr) {
  return Interpolation::cubic(wrapf(readptr, bufferLength), buffer, bufferLength);
}

float BufferH::readSample(float readptr) {
  return Interpolation::hermetic(wrapf(readptr, bufferLength), buffer, bufferLength);
}

