#include "./buffer.hpp"
#include "dsp.h"

using namespace dspheaders;

float BufferL::readSample(float readptr) {
  return Interpolation::linear(
    wrapf(readptr, bufferlength), buffer
  );
}


float BufferC::readSample(float readptr) {
  return Interpolation::cubic(
    wrapf(readptr, bufferlength), buffer, bufferlength
  );
}

float BufferH::readSample(float readptr) {
  return Interpolation::hermetic(
    wrapf(readptr, bufferlength), buffer, bufferlength
  );
}

