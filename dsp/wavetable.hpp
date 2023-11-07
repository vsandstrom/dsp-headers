#pragma once

#include "dsp.h"
#include "interpolation.hpp"


namespace dspheaders {

  enum WAVESHAPE {
    SINE,
    TRIANGLE,
    SAW,
    REVSAW,
    SQUARE,
    REVSQUARE,
    HANNING,
    ENV
  };

  class Wavetable {
    private:
    float readptr;
    float samplerate;
    float* table;
    unsigned tablelength;

    float (*interpolate)(float, float*, unsigned) = nullptr;

    // generates the next sample
    float read();
    // generates the next sample with phase offset
    float read(float phase);
    void populatetable(WAVESHAPE waveshape);

    public:
    float frequency;

    // Generate the next sample from the wavetable oscillator
    float play();
    // Generate the next sample from the wavetable oscillator.
    //
    // Allows for phase modulation (FM).
    float play(float phase);


    // WAVESHAPE waveshape - Takes an enum choosing among simple waveforms as first argument.
    //
    // unsigned int tablelength - length in number of samples in table
    //
    // unsigned int samplerate - audio engine setting of samples per seconds, usually 44100 or
    // 48000
    //
    // float (*interpolate) - supply a pointer to specified interpolation function
    //
    // (interpolation::cubic is not suitable for SQUARE and SAW because smoothing discontinuities
    // can result in wierd sample spikes.)
    Wavetable(
      WAVESHAPE waveshape,
      unsigned tablelength,
      unsigned samplerate,
      float (*interpolate)(float, float*, unsigned)
    );

    // float* table - an pregenerated table.
    //
    // unsigned int tablelength - length in number of samples in table
    //
    // unsigned int samplerate - audio engine setting of samples per seconds, usually 44100 or
    // 48000
    //
    // float (*interpolate) - supply a pointer to specified interpolation function
    //
    // (interpolation::cubic is not suitable for SQUARE and SAW because smoothing discontinuities
    // can result in wierd sample spikes.)
    Wavetable(
      float* table,
      unsigned tablelength,
      unsigned samplerate,
      float (*interpolate)(float, float*, unsigned)
    );
  };
} // namespace dspheaders
