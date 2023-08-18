#include "wavetable.hpp"
#include "dsp.h"
#include "waveshape.h"
#include <algorithm>

using namespace dspheaders;

Wavetable::Wavetable(
  WAVESHAPE waveshape,
  unsigned tablelength,
  unsigned samplerate,
  float (*interpolate)(float pos, float* table, unsigned length)
): tablelength(tablelength), samplerate((float)samplerate), interpolate(interpolate) {
  table = new float[tablelength+1];
  populatetable(waveshape);
  position = 0.f;
};

Wavetable::Wavetable(
  float* table,
  unsigned tablelength,
  unsigned samplerate,
  float (*interpolate)(float pos, float* table, unsigned length)
): table(table), tablelength(tablelength), samplerate((float)samplerate), interpolate(interpolate) {
  position = 0.f;
};


void Wavetable::movepointer(float phase) {
  // Make sure phase of the input signal is contained between 0 and 1.f
	float normalizedPhase = clamp((phase+1) * 0.5, 0.f, 1.f);
  wrapf(&(position += tablelength / (samplerate / (frequency * normalizedPhase))), tablelength);
}

void Wavetable::movepointer() {
  wrapf(&(position += tablelength / (samplerate / frequency)), tablelength);
}

float Wavetable::play(){
  float out = interpolate(position, table, tablelength);
  movepointer();
  return out;
}

float Wavetable::play(float phase){
  float out = interpolate(position, table, tablelength);
  movepointer(phase);
  return out;
}


void Wavetable::populatetable(WAVESHAPE waveshape) {
  float inc = 0.f, angle = 0.f, numSamples = (float) tablelength;
  switch (waveshape) {
    case (SINE): { sine(table, tablelength); break; }
    case (SAW): { saw(table, tablelength); break; }
    case (TRIANGLE): { triangle(table, tablelength); break; }
    case (SQUARE): { square(table, tablelength); break; }
    case (HANNING): { hanning(table, tablelength); break; }
    case (ENV): { hanning(table, tablelength); break; }
  }
}
