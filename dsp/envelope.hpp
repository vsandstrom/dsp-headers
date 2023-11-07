#include "dsp.h"
#include "buffer.hpp"
#include <cmath>

/*  
 *  TODO:
 *  [ ] - apply curve on each envelope segment
 *  [ ] - write test to make sure each value in points list is >= 0
 *  [ ] - dsp.h function that finds closest pow2
 *
 * BaseEnvelope could probably be a descendent of Buffer, or Buffer could 
 * perhaps have a bunch of extra functions to give it this functionality.
 * */

namespace dspheaders {
  enum GATE {
    on = 1, 
    off = 0,
    cycle = 2,
  };


  class Envelope {
    protected:
      Buffer buffer;
      float* points;
      float* times;
      float* curves;
      unsigned pLen;
      unsigned tLen;
      unsigned cLen;
      float samplerate;
      float prev;
      float readptr;
      void generate();
      void generateCurve();
    public: 
      Envelope(
        float* points,
        unsigned pLen,
        float* times,
        unsigned tLen,
        float samplerate,
        float (*interpolate)(float, float*, unsigned)
      );

      Envelope(
        float* points,
        unsigned pLen,
        float* times,
        unsigned tLen,
        float* curves,
        unsigned cLen,
        float samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
      // Returns current value from table
      // float play();

      // Resets envelope to start and returns the first value from table
      float play(GATE trigger);
      float play(GATE trigger, float speed);
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


