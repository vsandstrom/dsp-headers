#pragma once
#include <cstddef>

/* ˙ˆ˙ */

/*
 * TODO: 
 * [  ] - Make buffers resize itself if atk/rel-values in Envelope changes for example
 */

namespace dspheaders {
// Self-wrapping and interpolating buffers
  class Buffer{
    private:
      // Pointer to assigned interpolation function
      float (*interpolate)(float, float*, size_t);
    public:
      float* buffer = nullptr;
      unsigned bufferlength = 0;
      float position;
      float samplerate;

      // Read a interpolated float based on position  
      float readsample(float readptr) {
        return interpolate(wrapf(&readptr, bufferlength), buffer, bufferlength); 
      };

      // Read a sample of a floored position value
      float readsample(unsigned readptr) {
        return interpolate(wrap(&readptr, bufferlength), buffer, bufferlength);
      }

      // write a sample to a position in the buffer
      void writesample(float sample, int writeptr) {
        buffer[wrap(&writeptr, bufferlength)] = sample;
      };

      // Zero out the assigned buffer array
      //
      // important for smaller systems that do not clear old memory
      void initbuffer() {
        dspheaders::initbuffer(buffer, bufferlength+1);
      }
      
      Buffer(
        float* buffer,
        unsigned buflength,
        float samplerate,
        float (*interpolate)(float, float*, unsigned)
      );

      // Initialize buffer based on a duration in seconds
      Buffer(
        float seconds,
        unsigned samplerate,
        float (*interpolate)(float, float*, size_t)
      );

      // Initialize buffer based on a duration in samples
      template<unsigned N>
      Buffer(
        unsigned samples,
        unsigned samplerate,
        float (*interpolate)(float, float*, size_t)
      );
  };
  
}
