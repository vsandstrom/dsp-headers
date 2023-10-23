#include "buffer.hpp"
#include "dsp.h"
#include "dsp_math.h"
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
  // important for smaller systems that do not clear old memory
  initbuffer();
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

  buffer = new float[bufferlength];
}

Buffer::~Buffer() {
    delete[] buffer;
}


const float Buffer::readsample(unsigned readptr) {
  return buffer[wrap(readptr, bufferlength)]; 
};

const float * Buffer::getReadPtr(unsigned readptr) {
    return &buffer[readptr];
}

void Buffer::writesample(float sample, int writeptr) {
  buffer[wrap(&writeptr, bufferlength)] = sample;
};


void Buffer::init() {
    for (int i = 0; i < bufferlength; ++i) {
        buffer[i] = 0.f;
    }
}
