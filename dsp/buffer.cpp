#include "./buffer.hpp"
#include "dsp.h"

using namespace dspheaders;
// BaseBuffer::BaseBuffer(float seconds, unsigned int samplerate) {
//   bufferLength = samplerate * seconds;
//   // To simplify interpolation, table is made n+1 sample long
//   buffer = new float[bufferLength+1];
// }

// float BaseBuffer::readSample(int readptr) {
//   return buffer[wrap(readptr, bufferLength)];
// }

float BufferL::readSample(float readptr) {
  return Interpolation::linear(wrapf(readptr, bufferLength), buffer);
}

float BufferL2::readSample(float readptr) {
  return Interpolation::bilinear(wrapf(readptr, bufferLength), buffer, bufferLength);
}

float BufferC::readSample(float readptr) {
  return Interpolation::cubic(wrapf(readptr, bufferLength), buffer, bufferLength);
}

float BufferH::readSample(float readptr) {
  return Interpolation::hermetic(wrapf(readptr, bufferLength), buffer, bufferLength);
}

// void BaseBuffer::writeSample(float sample, int ptr) {
//   buffer[wrap(ptr, bufferLength)] = sample;
// }
