#include "./buffer.hpp"

using namespace dspheaders;
BaseBuffer::BaseBuffer(float seconds, unsigned int samplerate) {
  bufferLength = samplerate * seconds;
  buffer = new float[bufferLength+1];
}

float BaseBuffer::readSample(int readptr) {
  return buffer[readptr];
}

float BufferL::readSample(float readptr) {
  return Interpolation::linear(readptr, buffer);
}

float BufferC::readSample(float readptr) {
  return Interpolation::cubic(readptr, buffer, bufferLength);
}

void BaseBuffer::writeSample(float sample, int ptr) {
  buffer[ptr] = sample;
}
