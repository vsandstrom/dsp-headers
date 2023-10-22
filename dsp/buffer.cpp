#include "buffer.hpp"
#include "dsp.h"
#include "dsp_math.h"
#include "interpolation.hpp"

using namespace dspheaders;

Buffer::Buffer(
  unsigned size,
  unsigned samplerate
  )
: bufferlength(size) {

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
