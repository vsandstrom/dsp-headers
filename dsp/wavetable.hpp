#pragma once
#include <cstdlib>

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

    float (*interpolate)(float, float*, size_t) = nullptr;

    // generates the next sample
    float read();
    // generates the next sample with phase offset
    //
    // phase input is clamped to be within 0.f and 1.f
    float read(float phase);
    // Generates a wavetable according to the waveshape enum argument 
    void populatetable(WAVESHAPE waveshape);

    public:
    float frequency;

    // Generate the next sample from the wavetable oscillator
    float play();
    // Generate the next sample from the wavetable oscillator.
    //
    // Allows for phase modulation (FM).
    float play(float phase);

    // Returns number of samples in table
    unsigned getTablelength();


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
      float (*interpolate)(float, float*, size_t)
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
      float (*interpolate)(float, float*, size_t)
    );
  };
} // namespace dspheaders
