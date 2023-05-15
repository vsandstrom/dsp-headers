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
      void write(float sample);
      float read(float speed);
      float play(float sample, float speed);
  };
}

#endif
