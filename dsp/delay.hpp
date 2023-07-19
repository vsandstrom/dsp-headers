#pragma once

#include "dsp.h"
#include "buffer.hpp"

// TODO: 
// [  ] - interpolation of the delaytime, change of time could be smoother? 

namespace dspheaders {
  template <typename T>
  class BaseDelay {
    protected:
      T buffer;
      // float* buffer;
      unsigned int samplerate;
      unsigned int delay_taps = 1;
      int writeptr = 0;

      inline void initBuffer() {
        for (int i = 0; i < buffer.bufferLength; ++i) {
          buffer.buffer[i] = 0.0f;
        }
      }

    public:
      float time = 0.2f;        // Default time time 0.2 seconds
      // Constructor:

      BaseDelay(unsigned int samplerate, float time, unsigned int delay_taps): 
        buffer(T(time * 2, samplerate)),
        time(time),
        samplerate(samplerate)
      {
        initBuffer();
      }
      // Easiest way to init, uses ready-made, preinitialized memory
      BaseDelay(unsigned int samplerate, T buffer, unsigned int delay_taps):
        buffer(buffer),
        samplerate(samplerate) {
        initBuffer();
      }

      void taps(float taps) {
        delay_taps = taps;
      }

      void delaytime(float delaytime) {
        time = delaytime;
      }

      // Writes the current sample,
      // ----
      //
      // Uses linear interpolation to write signal to the time buffer.
      void write(float sample) {
        // Within bounds-checking is handled in the Buffer object
        buffer.writeSample(sample, writeptr);
        writeptr++;
      }

      // Reads from time buffer.
      // ----
      //
      // float time - Duration before first time bounce.
      float read(float delaytime) {
        // Non-interpolating read function
        float output = 0.f;
        int tapdelaytime = (delaytime * samplerate);
        for (int i = 1; i <= delay_taps; i++) {
          int tap = writeptr - (tapdelaytime * i);
          // Within bounds-checking is handled in the Buffer object
          output += buffer.readSample(tap);
        }
        return output;
      }

      // End-point for delay class.
      // ----
      //
      // float input - Input sample to write to buffer.
      // float time - Length before first time.
      // float wet - Dry / wet balance.
      // float feedback - Amount of output reintroduced into time buffer.
      float play(float input, float delaytime, float wet, float feedback) {
        float output = read(delaytime);
        // write the time back to write head with feedback
        write(input + ( output * feedback ));
        // wet controls dry/wet balance of time
        return output * wet;
      }

  };

  class Delay: public BaseDelay<Buffer> {
    public:
      Delay(unsigned int samplerate, float time, unsigned int delay_taps) : 
        BaseDelay<Buffer>(samplerate, time, delay_taps) {};

      // Easiest way to init, uses ready-made, preinitialized memory
      Delay(unsigned int samplerate, Buffer buffer, unsigned int delay_taps) : 
        BaseDelay<Buffer>(samplerate, buffer, delay_taps) {};
      float read(float time);
  };
  
  class DelayL: public BaseDelay<BufferL> {
    public:
      DelayL(unsigned int samplerate, float time, unsigned int delay_taps) : 
        BaseDelay<BufferL>(samplerate, time, delay_taps) {};
      // Easiest way to init, uses ready-made, preinitialized memory
      DelayL(unsigned int samplerate, BufferL buffer, unsigned int delay_taps) :
        BaseDelay<BufferL>(samplerate, buffer, delay_taps) {};
      float read(float time);
  };
  
  // class DelayL2: public BaseDelay<BufferL2> {
  //   public:
  //     DelayL2(unsigned int samplerate, float time, unsigned int delay_taps) :
  //       BaseDelay<BufferL2>(samplerate, time, delay_taps) {};
  //     // Easiest way to init, uses ready-made, preinitialized memory
  //     DelayL2(unsigned int samplerate, BufferL2 buffer, unsigned int delay_taps) : 
  //       BaseDelay<BufferL2>(samplerate, buffer, delay_taps) {};
  //     float read(float time);
  // };
  
  class DelayC: public BaseDelay<BufferC> {
    public:
      DelayC(unsigned int samplerate, float time, unsigned int delay_taps) :
        BaseDelay<BufferC>(samplerate, time, delay_taps) {};
      // Easiest way to init, uses ready-made, preinitialized memory
      DelayC(unsigned int samplerate, BufferC buffer, unsigned int delay_taps) : 
        BaseDelay<BufferC>(samplerate, buffer, delay_taps) {};
      float read(float time);
  };
  
  class DelayH: public BaseDelay<BufferH> {
    public:
      DelayH(unsigned int samplerate, float time, unsigned int delay_taps) :
        BaseDelay<BufferH>(samplerate, time, delay_taps) {};
      // Easiest way to init, uses ready-made, preinitialized memory
      DelayH(unsigned int samplerate, BufferH buffer, unsigned int delay_taps) : 
        BaseDelay<BufferH>(samplerate, buffer, delay_taps) {};
      float read(float time);
  };

} // namespace dspheaders
