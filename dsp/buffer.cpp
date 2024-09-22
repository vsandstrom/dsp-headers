
/* ˙ˆ˙ */
#include "buffer.hpp"
#include "dsp_math.h"
#include <cstddef>


using namespace dspheaders;

Buffer::Buffer(
  float seconds,
  unsigned samplerate,
  float (*interpolate)(float, float*, size_t))
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
  float (*interpolate)(float, float*, size_t))
: bufferlength(size), interpolate(interpolate) {

  if (bufferlength < 4) {
    // Allow for mini-buffers, but still not in conflict with 
    // interpolation
    bufferlength = 4;
  }
  buffer = new float[bufferlength+1];
  // important for smaller systems that do not clear old memory
  initbuffer();
}


float Buffer::readsample(float readptr) {
  return interpolate(wrapf(&readptr, bufferlength), buffer, bufferlength); 
};

float Buffer::readsample(unsigned readptr) {
  return interpolate(wrap(&readptr, bufferlength), buffer, bufferlength);
}

void Buffer::writesample(float sample, int writeptr) {
  buffer[wrap(&writeptr, bufferlength)] = sample;
};

// Experimental interpolated write
// void Buffer::writesample(float sample, float writeptr) {
//   writeinterpolation::linear(sample, wrapf(&writeptr, bufferlength), buffer, bufferlength);
// }

// important for smaller systems that do not clear old memory
