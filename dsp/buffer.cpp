#include "buffer.hpp"
#include "dsp.h"
#include "interpolation.hpp"

using namespace dspheaders;

Buffer::Buffer(
  float seconds,
  unsigned samplerate,
  float (*interpolate)(float, float*, unsigned))
: bufferlength(seconds*samplerate), interpolate(interpolate) {

  if (bufferlength < 4) {
    // Allow for mini-buffers, but still not in conflict with 
    // interpolation
    bufferlength = 4;
  }
  buffer = new float[bufferlength+1];
}

Buffer::Buffer(
  unsigned size,
  unsigned samplerate,
  float (*interpolate)(float, float*, unsigned))
: bufferlength(size), interpolate(interpolate) {

  if (bufferlength < 4) {
    // Allow for mini-buffers, but still not in conflict with 
    // interpolation
    bufferlength = 4;
  }
  buffer = new float[bufferlength+1];
}


float Buffer::readsample(float readptr) {
  return interpolate(wrapf(&readptr, bufferlength), buffer, bufferlength);
};

float Buffer::readsample(int readptr) {
  return interpolate(wrap(&readptr, bufferlength), buffer, bufferlength);

}

void Buffer::writesample(float sample, int writeptr) {
  buffer[wrap(&writeptr, bufferlength)] = sample;
};

void Buffer::initbuffer() {
  for (unsigned i = 0; i < bufferlength; i++) {
    buffer[i] = 0.f;
  }
}
