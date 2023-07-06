#include <cstdint>
#include "dsp.h"
#include "buffer.hpp"

#ifndef DELAY_HPP
#define DELAY_HPP

namespace dspheaders {
  class Delay {
    private:
      Buffer buffer;
      // float* buffer;
      uint32_t samplerate;
      uint32_t delay_taps = 1;
      int writeptr = 0;

      inline void initBuffer() {
        for (int i = 0; i < buffer.bufferLength; ++i) {
          buffer.buffer[i] = 0.0f;
        }
      }

    public:
      float delay = 0.2f;        // Default delay time 0.2 seconds
      // Constructor:
      Delay(uint32_t samplerate, float delay, uint32_t delay_taps);

      // Easiest way to init, uses ready-made, preinitialized memory
      Delay(uint32_t samplerate, Buffer buffer, uint32_t delay_taps);

      // Writes the current sample,
      // ----
      //
      // Uses linear interpolation to write signal to the delay buffer.
      void write(float sample);

      // Reads from delay buffer.
      // ----
      //
      // float time - Duration before first delay bounce.
      float read(float time);

      float readInterpolated(float time);

      // End-point for delay class.
      // ----
      //
      // float input - Input sample to write to buffer.
      // float time - Length before first delay.
      // float wet - Dry / wet balance.
      // float feedback - Amount of output reintroduced into delay buffer.
      float play(float input, float time, float wet, float feedback);

  };
} // namespace dspheaders

#endif
