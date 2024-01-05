#pragma once

/*
 *
 * TODO: 
 * [  ] - Make buffers resize itself if atk/rel-values in Envelope changes for example
 */
namespace dspheaders {
// Self-wrapping and interpolating Buffer
  class Buffer{
    private:
      // Pointer to assigned interpolation function
      float (*interpolate)(float, float*, unsigned);
    public:
      float* buffer;
      unsigned bufferlength;

      // Read a interpolated float based on position  
      float readsample(float readptr);
      // Read a sample of a floored position value
      float readsample(unsigned readptr);
      // write a sample to a position in the buffer
      void writesample(float sample, int writeptr);
      
      // void writesample(float sample, float writeptr);

      // Zero out the assigned buffer array
      //
      // important for smaller systems that do not clear old memory
      void initbuffer();

      // Initialize buffer based on a duration in seconds
      Buffer(
        float seconds,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );

      // Initialize buffer based on a duration in samples
      Buffer(
        unsigned samples,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
  };
  
}
