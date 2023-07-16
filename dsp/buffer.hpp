#pragma once

#include "dsp.h"
#include "interpolation.hpp"
#include <cmath>
#include <iostream>

namespace dspheaders {
  // Self-wrapping and interpolating Buffer
  template<typename T>
  class BaseBuffer {
    public: 
      T* buffer;
      unsigned int bufferLength;
      BaseBuffer(float seconds, unsigned int samplerate) : bufferLength(seconds * samplerate) {
        // To simplify interpolation, table is made n+1 sample long
        if (bufferLength < 4) {
          // Allow for mini-buffers, but still not in conflict with interpolation
          bufferLength = 4;
        }
        buffer = new T[bufferLength+1];
      };
      // reading bck in buffer: n++ - delay
      float readSample(int readptr) {
        return buffer[wrap(readptr, bufferLength)];
      };
      // Writing fwd in buffer: n++
      void writeSample(T sample, int writeptr) {
        buffer[wrap(writeptr, bufferLength)] = sample;
      };
  };

  // Self-wrapping and interpolating buffer for audio DSP
  class Buffer: public BaseBuffer<float> { 
    public: 
      Buffer(float seconds, unsigned int samplerate) : BaseBuffer(seconds, samplerate) {};
  };

  // Self-wrapping and interpolating buffer for audio DSP
  class BufferL: public BaseBuffer<float> {
    public:
      BufferL(float seconds, unsigned int samplerate) : BaseBuffer(seconds, samplerate) {};
      float readSample(float readptr);
  };

  // Self-wrapping and interpolating buffer for audio DSP
  class BufferL2: public BaseBuffer<float> {
    public:
      BufferL2(float seconds, unsigned int samplerate) : BaseBuffer(seconds, samplerate) {};
      float readSample(float readptr);
  };
  
  class BufferC: public BaseBuffer<float> {
    public:
      BufferC(float seconds, unsigned int samplerate) : BaseBuffer(seconds, samplerate) {};
      float readSample(float readptr);
  };

  class BufferH: public BaseBuffer<float> {
    public:
      BufferH(float seconds, unsigned int samplerate) : BaseBuffer(seconds, samplerate) {};
      float readSample(float readptr);
  };
}
