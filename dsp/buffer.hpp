#include "dsp.h"
#include "interpolation.hpp"
#include <cmath>
#include <iostream>
#ifndef BUFFER_HPP
#define BUFFER_HPP

namespace dspheaders {
  class BaseBuffer {
    public: 
      float* buffer;
      uint32_t bufferLength;
      BaseBuffer(float seconds, unsigned int samplerate);
      // reading bck in buffer: n++ - delay
      virtual float readSample(int readptr);
      float readInterpolatedSample(float readptr);
      // Writing fwd in buffer: n++
      void writeSample(float sample, int writeptr);
  };

  class Buffer: public BaseBuffer { 
    public: 
      Buffer(float seconds, unsigned int samplerate);
  };

  class BufferL: public BaseBuffer {
    public:
      BufferL(float seconds, unsigned int samplerate);
      float readSample(float readptr);
  };

  class BufferC: public BaseBuffer {
    public:
      BufferC(float seconds, unsigned int samplerate);
      float readSample(float readptr);
  };
}

#endif
