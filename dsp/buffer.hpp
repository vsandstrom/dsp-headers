#include "dsp.h"
#include "interpolation.hpp"
#include <cmath>
#include <iostream>
#ifndef BUFFER_HPP
#define BUFFER_HPP

namespace dspheaders {
  class Buffer {
    public: 
      float* buffer;
      uint32_t bufferLength;
      Buffer(float seconds, uint32_t samplerate);
      // reading bck in buffer: n++ - delay
      float readSample(int readptr);

      // Writing fwd in buffer: n++
      void writeSample(float sample, int writeptr);
  };
}

#endif
