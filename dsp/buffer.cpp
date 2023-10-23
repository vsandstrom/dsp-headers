#include "buffer.hpp"
#include "dsp.h"
#include "dsp_math.h"
#include "interpolation.hpp"

using namespace dspheaders;

Buffer::Buffer(
  float seconds,
  unsigned samplerate,
  float (*interpolate)(float, float*, unsigned))
: bufferlength(findpow2(seconds*samplerate)), interpolate(interpolate) {

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
: bufferlength(findpow2(size)), interpolate(interpolate) {

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
  return interpolate(wrap_dangerously(&readptr, bufferlength), buffer, bufferlength);
}

void Buffer::writesample(float sample, int writeptr) {
  buffer[wrap(&writeptr, bufferlength)] = sample;
};

// Experimental interpolated write
// void Buffer::writesample(float sample, float writeptr) {
//   writeinterpolation::linear(sample, wrapf(&writeptr, bufferlength), buffer, bufferlength);
// }

// important for smaller systems that do not clear old memory
void Buffer::initbuffer() {
  dspheaders::initbuffer(buffer, bufferlength+1);
}
