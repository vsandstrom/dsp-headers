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


  class Envelope {
    protected:
      Buffer buffer;
      float* breakpoints;
      float* breaktimes;
      unsigned pointlength;
      unsigned timeslength;
      float samplerate;
      float readptr;
      void generate();
    public: 
      Envelope(
        float* breakpoints,
        unsigned pointlength,
        float* breaktimes,
        unsigned timeslength,
        float samplerate,
        float (*interpolate)(float, float*, unsigned)
      );

      // Returns current value from table
      // float play();

      // Resets envelope to start and returns the first value from table
      float play(GATE trigger);

      void repr();
  };

  class PercEnv {
    private:
      Buffer buffer;
      float attack;
      float decay;
      float samplerate;
      float readptr;
      void generate();
    public: 
      float play();
      float play(GATE trigger);
      PercEnv(float attack, float decay, float samplerate, float (*interpolate)(float, float*, unsigned));
  };
} // namespace dspheaders
  //


