#include "./buffer.hpp"

using namespace dspheaders;
Buffer::Buffer(float seconds, uint32_t samplerate) {
  bufferLength = samplerate * seconds;
  buffer = new float[bufferLength];
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
