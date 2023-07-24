#pragma once

#include "dsp.h"
#include "interpolation.hpp"
#include <cmath>
#include <iostream>

/*
 * TODO: 
 * [  ] - Make buffers resize itself if atk/rel-values in Envelope changes for example
 */

namespace dspheaders {
  // Self-wrapping and interpolating Buffer
  template<typename T>
  class BaseBuffer {
    protected:
      T* buffer;
    public: 
      unsigned int bufferlength;
      BaseBuffer(float seconds, unsigned int samplerate) 
        : bufferlength(seconds * samplerate) {
        // To simplify interpolation, table is made n+1 sample long
        if (bufferlength < 4) {
          // Allow for mini-buffers, but still not in conflict with 
          // interpolation
          bufferlength = 4;
        }
        buffer = new T[bufferlength+1];
      };
      // reading bck in buffer: n++ - delay
      float readSample(int readptr) {
        return buffer[wrap(readptr, bufferlength)];
      };
      // Writing fwd in buffer: n++
      void writeSample(T sample, int writeptr) {
        buffer[wrap(writeptr, bufferlength)] = sample;
      };

      inline void initBuffer() {
        for (int i = 0; i < bufferlength; ++i) {
          buffer[i] = 0.0f;
        }
      }
  };

  // Self-wrapping and interpolating buffer for audio DSP
  class Buffer: public BaseBuffer<float> { 
    public: 
      Buffer(float seconds, unsigned int samplerate) 
        : BaseBuffer(seconds, samplerate) {};
  };

  // Self-wrapping and interpolating buffer for audio DSP
  class BufferL: public BaseBuffer<float> {
    public:
      BufferL(float seconds, unsigned int samplerate) 
        : BaseBuffer(seconds, samplerate) {};
      float readSample(float readptr);
  };

  class BufferC: public BaseBuffer<float> {
    public:
      BufferC(float seconds, unsigned int samplerate) 
        : BaseBuffer(seconds, samplerate) {};
      float readSample(float readptr);
  };

  class BufferH: public BaseBuffer<float> {
    public:
      BufferH(float seconds, unsigned int samplerate) 
        : BaseBuffer(seconds, samplerate) {};
      float readSample(float readptr);
  };
}
