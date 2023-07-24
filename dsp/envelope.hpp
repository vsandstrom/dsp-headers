#include "dsp.h"
#include "buffer.hpp"
#include <cmath>

/*  
 *  TODO:
 *  [ ] - apply curve on each envelope segment
 *  [ ] - write test to make sure each value in breakpoints list is >= 0
 *  [ ] - dsp.h function that finds closest pow2
 *
 * BaseEnvelope could probably be a descendent of Buffer, or Buffer could 
 * perhaps have a bunch of extra functions to give it this functionality.
 * */

namespace dspheaders {
  enum GATE {
    on = 1, 
    off = 0,
  };


  template<typename T>
  class BaseEnvelope {
    protected:
      T* buffer = nullptr;
      float* breakpoints;
      float* breaktimes;
      unsigned pointlength;
      unsigned timeslength;
      unsigned samplerate;
      float readptr = 0.f;
      void populateEnvelope() {
        float* pts = breakpoints;
        T* b = buffer;
        for (unsigned i = 1; i < pointlength; ++i) {
          // works because breakpoints need to be only positive. 
          // needs testing at construction
          float slope = breakpoints[i-1] - breakpoints[i];
          unsigned numsamples = breaktimes[i-1] * samplerate;
          float inc = slope / (float)numsamples;
          for (unsigned i = 0; i < numsamples; ++i) {
            // write function to apply curve on envelope segment here.
            *b -> buffer++ = i * inc;
          }
        }
      };
    public: 
      BaseEnvelope(
          float* breakpoints,
          unsigned pointlength,
          float* breaktimes,
          unsigned timeslength,
          unsigned samplerate) : 
        breakpoints(breakpoints),
        pointlength(pointlength),
        breaktimes(breaktimes),
        timeslength(timeslength) {
        float time = 0.f;
        for (unsigned int i = 0; i < timeslength; ++i) {
          // convert seconds to a length of samples
          time += breaktimes[i];
          // breaktimes[i] = size;
          // Find minimal pow2 size of buffer
        } 

        // quickfix to make sure that duration of envelope can be contained in 
        // buffer
        time += 0.5f;
        *buffer = T(time, samplerate);
      }

      // Returns current value from table
      float play() {
      };

      // Resets envelope to start and returns the first value from table
      float play(GATE trigger) {
        if (trigger) {
          readptr = 0.f;
        }
      };
  };

  class Envelope : public BaseEnvelope<Buffer> {
    Envelope(
      float* breakpoints, unsigned pointlength, 
      float* breaktimes, unsigned timeslength, 
      unsigned samplerate
      );
  };

  class EnvelopeL : public BaseEnvelope<BufferL> {
    EnvelopeL(
      float* breakpoints, unsigned pointlength, 
      float* breaktimes, unsigned timeslength, 
      unsigned samplerate
      );
  };

  class EnvelopeC : public BaseEnvelope<BufferC> {
    EnvelopeC(
      float* breakpoints, unsigned pointlength,
      float* breaktimes, unsigned timeslength,
      unsigned samplerate
      );
  };

  class EnvelopeH : public BaseEnvelope<BufferH> {
    EnvelopeH(
      float* breakpoints, unsigned pointlength,
      float* breaktimes, unsigned timeslength,
      unsigned samplerate
      );
  };
} // namespace dspheaders
