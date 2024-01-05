#include "wavetable.hpp"
#include "dsp.h"
#include "waveshape.h"

using namespace dspheaders;



Wavetable::Wavetable(
  WAVESHAPE waveshape,
  unsigned tablelength,
  unsigned samplerate,
  float (*interpolate)(float pos, float* table, unsigned length)
): tablelength(tablelength), samplerate((float)samplerate), interpolate(interpolate) {
  table = new float[tablelength+1];
  initbuffer(table, tablelength+1);

  populatetable(waveshape);
  readptr = 0.f;
};

Wavetable::Wavetable(
  float* table,
  unsigned tablelength,
  unsigned samplerate,
  float (*interpolate)(float pos, float* table, unsigned length)
): table(table), tablelength(tablelength), samplerate((float)samplerate), interpolate(interpolate) {
  readptr = 0.f;
};

float Wavetable::read() {
  wrapf(&(readptr += tablelength / (samplerate / frequency)), tablelength);
  return interpolate(readptr, table, tablelength);
}

float Wavetable::read(float phase) {
  // Make sure phase of the input signal is contained between 0 and 1.f
	float normalizedPhase = clamp((phase+1) * 0.5, 0.f, 1.f);
  wrapf(&(readptr += tablelength / (samplerate / (frequency * normalizedPhase))), tablelength);
  return interpolate(readptr, table, tablelength);
}

float Wavetable::play(){
  // avoid aliasing, freq bigger than nyqvist
  if (frequency > (samplerate / 2)) return 0.f;
  return read();
}

float Wavetable::play(float phase){
  // avoid aliasing - when used in FM, aliasing should be handled more 
  if (frequency > (samplerate / 2)) return 0.f;
  return read(phase);
}

unsigned Wavetable::getTablelength() {
  return tablelength;
}

void Wavetable::populatetable(WAVESHAPE waveshape) {
  float inc = 0.f, angle = 0.f, numSamples = (float) tablelength;
  switch (waveshape) {
    case (SINE): { sine(table, tablelength); break; }
    case (SAW): { saw(table, tablelength); break; }
    case (REVSAW): { revsaw(table, tablelength); break; }
    case (TRIANGLE): { triangle(table, tablelength); break; }
    case (SQUARE): { square(table, tablelength); break; }
    case (REVSQUARE): { revsquare(table, tablelength); break; }
    case (HANNING): { hanning(table, tablelength); break; }
    case (ENV): { hanning(table, tablelength); break; }
  }
}
