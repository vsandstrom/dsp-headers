#include <cstdint>
#include "dsp.h"
#include "buffer.hpp"

#ifndef DELAY_HPP
#define DELAY_HPP

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
      float delay = 0.2f;        // Default delay time 0.2 seconds
      // Constructor:
      BaseDelay(unsigned int samplerate, float delay, unsigned int delay_taps);

      // Easiest way to init, uses ready-made, preinitialized memory
      BaseDelay(unsigned int samplerate, T buffer, unsigned int delay_taps);

      // Writes the current sample,
      // ----
      //
      // Uses linear interpolation to write signal to the delay buffer.
      void write(float sample);

      // Reads from delay buffer.
      // ----
      //
      // float time - Duration before first delay bounce.
      virtual float read(float time);

      // float readInterpolated(float time);

      // End-point for delay class.
      // ----
      //
      // float input - Input sample to write to buffer.
      // float time - Length before first delay.
      // float wet - Dry / wet balance.
      // float feedback - Amount of output reintroduced into delay buffer.
      float play(float input, float time, float wet, float feedback);

  };

  class Delay: public BaseDelay<Buffer> {
    public:
      Delay(unsigned int samplerate, float delay, unsigned int delay_taps) : BaseDelay<Buffer>(samplerate, delay, delay_taps) {};

      // Easiest way to init, uses ready-made, preinitialized memory
      Delay(unsigned int samplerate, Buffer buffer, unsigned int delay_taps) : BaseDelay<Buffer>(samplerate, buffer, delay_taps) {};
      float read(float time) override;
  };
  
  class DelayL: public BaseDelay<BufferL> {
    public:
      DelayL(unsigned int samplerate, float delay, unsigned int delay_taps) : BaseDelay<BufferL>(samplerate, delay, delay_taps) {};
      // Easiest way to init, uses ready-made, preinitialized memory
      DelayL(unsigned int samplerate, BufferL buffer, unsigned int delay_taps) : BaseDelay<BufferL>(samplerate, buffer, delay_taps) {};
      float read(float time) override;
  };
  
  class DelayC: public BaseDelay<BufferC> {
    public:
      DelayC(unsigned int samplerate, float delay, unsigned int delay_taps) : BaseDelay<BufferC>(samplerate, delay, delay_taps) {};
      // Easiest way to init, uses ready-made, preinitialized memory
      DelayC(unsigned int samplerate, BufferC buffer, unsigned int delay_taps) : BaseDelay<BufferC>(samplerate, buffer, delay_taps) {};
      float read(float time) override;
  };


} // namespace dspheaders

#endif
