#pragma once

#include "dsp.h"
#include "interpolation.hpp"


namespace dspheaders {

  enum WAVESHAPE {
    SINE,
    TRIANGLE,
    SAW,
    SQUARE,
    ENV,
    HANNING
  };


  class Wavetable {
    private:
    float position;
    float samplerate;
    float* table;
    unsigned tablelength;

    float (*interpolate)(float, float*, unsigned) = nullptr;

    void movepointer();
    void movepointer(float phase);

    void populatetable(WAVESHAPE waveshape);

    public:
    float frequency;

    float play();
    float play(float phase);
    Wavetable(
      WAVESHAPE waveshape,
      unsigned tablelength,
      unsigned samplerate,
      float (*interpolate)(float, float*, unsigned)
    );

    Wavetable(
      float* table,
      unsigned tablelength,
      unsigned samplerate,
      float (*interpolate)(float, float*, unsigned)
    );

  };
} // namespace dspheaders


