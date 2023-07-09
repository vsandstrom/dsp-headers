#include "./buffer.hpp"

using namespace dspheaders;
Buffer::Buffer(float seconds, unsigned int samplerate) {
  bufferLength = samplerate * seconds;
  buffer = new float[bufferLength+1];
}

float Buffer::readSample(int readptr) {
  return buffer[readptr];
}

float Buffer::readInterpolatedSample(float readptr) {
  return Interpolation::linear(readptr, buffer);
}

void Buffer::writeSample(float sample, int ptr) {
  buffer[ptr] = sample;
}
