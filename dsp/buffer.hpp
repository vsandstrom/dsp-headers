/* ˙ˆ˙ */

#pragma once
#include <array>
#include <cstddef>
#include "dsp.h"

namespace dspheaders {
// Self-wrapping and interpolating Buffer
  template <size_t size> class Buffer{
    private:
      struct M {
        size_t length;
        std::array<float, size> buffer;
      } m;

    public:
      // Read a interpolated float based on position  
      template <float(*interpolate)(const float, const float* const, const size_t)> 
      float readsample(float readptr) {
        readptr = (readptr < m.length) ? readptr : readptr -= m.length;
        return interpolate(readptr, m.buffer.data(), m.length); 
      };

      // Read a sample of a floored position value
      template <float(*interpolate)(const float, const float* const, const size_t)> 
      float readsample(unsigned readptr) {
        readptr = (readptr < m.length) ? readptr : readptr -= m.length;
        return interpolate(readptr, m.buffer.data(), m.length);
      }
      // write a sample to a position in the m.buffer
      void writesample(float sample, int writeptr) {
        writeptr = (writeptr < m.length) ? writeptr : writeptr -= m.length;
        m.buffer[writeptr] = sample;
      };

      // void writesample(float sample, float writeptr);

      // Zero out the assigned m.buffer array
      //
      // important for smaller systems that do not clear old memory
      void initbuffer() {
        dspheaders::initbuffer(m.buffer.data(), m.length);
      }

      // Initialize m.buffer based on a duration in seconds
      Buffer(
        float seconds,
        unsigned samplerate,
        float (*interpolate)(float, float*, size_t)
      );

      // Initialize m.buffer based on a duration in samples
      Buffer(
        unsigned samples,
        unsigned samplerate,
        float (*interpolate)(float, float*, size_t)
      );
  };
  
}
