#pragma once
#include "dsp.h"
#include "buffer.hpp"
#include <cstddef>

namespace dspheaders {
  template <size_t VALUES, size_t DURATIONS>
  struct BreakPoints{
    float values[VALUES];
    float durations[DURATIONS];
    float curves[DURATIONS];
    BreakPoints(float values[VALUES], float durations[DURATIONS], float curves[DURATIONS]):
      values(values), durations(durations), curves(curves)
    { }
  };

  class Envelope {
    protected:
      float* buffer = nullptr;
      unsigned bufferlength = 0;
      float* points;
      float* times;
      float* curves;
      unsigned pLen;
      unsigned tLen;
      unsigned cLen;
      float samplerate;
      float prev;
      float readptr;
      float (*interpolate)(float, float*, unsigned);
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

      Envelope(
          float* table,
          unsigned tablelength,
          float samplerate,
          float (*interpolate)(float, float*, unsigned)
      );
      
      template<size_t VALUES, size_t DURATIONS>
      Envelope(
          BreakPoints<VALUES, DURATIONS> brk,
          float samplerate,
          float (*interpolate)(float, float*, unsigned)
        ): 
        points(brk.values),
        pLen(VALUES),
        times(brk.durations),
        tLen(DURATIONS),
        curves(brk.curves),
        cLen(DURATIONS),
        samplerate(samplerate),
        interpolate(interpolate)
      {
        generateCurve();
      };

      // Returns current value from table
      // float play();

      // Resets envelope to start and returns the first value from table
      float play();
      float play(float speed);
      float play(GATE trigger);
      float play(GATE trigger, float speed);

      float read(float ptr);

      unsigned length();
      bool running();
      bool finished();
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


