#include <cstdint>

#ifndef PI 
  #define PI 
  const float pi = 3.14159265358979323846f;
#endif

#ifndef DELAY_HPP
#define DELAY_HPP

namespace dspheaders {
  class Delay {
    private:
      float* buffer;
      uint32_t bufferLength;
      uint32_t samplerate;
      int writeptr = 0;
      float readptr = 0.f;
      float time = 0.2f;        // Default delay time 0.2 seconds

    public:
      float speed;
      // Constructor:
      Delay(uint32_t samplerate, float seconds);
      // Writes input signal to next sample in buffer
      // ----
      //
      // float sample - Current sample to be written to delay buffer
      void write(float sample);
      // Reads from delay buffer.
      // ----
      //
      // float delay - The amount of delay in seconds.
      // float speed - Speed of readptr, pitching up or down.
      float read(float delay, float speed);
      // End-point for delay class.
      // ----
      //
      // float delay - The amount of delay in seconds.
      // float speed - Speed of readptr, pitching up or down.
      // float input - Input sample to write to buffer.
      float play(float delay, float speed, float input);
  };
} // namespace dspheaders

#endif