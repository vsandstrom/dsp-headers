#pragma once

#include "dsp.h"
#include "buffer.hpp"

namespace dspheaders {

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


