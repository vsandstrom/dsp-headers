#include "grain.hpp"
#include "waveshape.h"
#include <cmath>

using namespace dspheaders;

/*
 *
 * TODO:
 * Should we keep track of all active / non-active grains
 * by bitencoding a 32bit number, capping number of grains
 * to 32 - would require finessing
 *
 * */

float Granulator::process(float offset) {
  int i = 0;
  float out = 0.f;
  // process all grains
  for (; i < maxgrains; i++) {
    // accumulate signals from all grains
    out += grains[i].play(offset);
  }
  // return signal
  return out;
}

// Default envelope - works fine to get going
Granulator::Granulator(
  float samplerate, 
  unsigned maxgrains,
  float (*interpolate)(float, float*, unsigned))
  : samplerate(samplerate),
  // Default grain envelope
    buffer(samplerate * 4, samplerate, interpolate) {

  // allocate memory for all grains
  grains = (Grain *)malloc(sizeof(Grain)*maxgrains);
  // if out of memory, return
  if (grains == nullptr) { return;} 
  for (int i = 0; i < maxgrains; i++) {
    // pass audio buffer and grain envelope by reference,
    grains[i] = Grain(0, 0.2, samplerate, &buffer, &grainenv);
  }
  // Create default envelope ( Hanning )
  float* env = new float[512];
  hanning(env, 512);
  grainenv = Envelope(env, 512, samplerate, interpolate);
};
   
    
// Envelope constructed from float array
Granulator::Granulator(
  float samplerate, 
  float* table, 
  unsigned tablelength,
  unsigned maxgrains,
  float (interpolate)(float, float*, unsigned))
  : samplerate(samplerate),
  // Construct envelope from given float array.
    grainenv(table, tablelength, samplerate, interpolate),
    buffer(samplerate * 4, samplerate, interpolate) { 
    // allocate memory for all grains
  grains = (Grain *)malloc(sizeof(Grain)*maxgrains);
  if (grains == nullptr) { return;} 
  for (int i = 0; i < maxgrains; i++) {
    // pass audio buffer and grain envelope by reference,
    grains[i] = Grain(0, 0.2, samplerate, &buffer, &grainenv);
  }
};

Granulator::~Granulator(){
  free(grains);
}
    
    
// Predefined envelope
Granulator::Granulator(float samplerate, Envelope grainEnvelope)
  : samplerate(samplerate),
    grainenv(grainEnvelope),
    buffer(samplerate * 4, samplerate, interpolation::linear) { 

  // allocate memory for all grains
  grains = (Grain *)malloc(sizeof(Grain)*maxgrains);
  if (grains == nullptr) { return;} 
  for (int i = 0; i < maxgrains; i++) {
    // pass audio buffer and grain envelope by reference,
    grains[i] = Grain(0, 0.2, samplerate, &buffer, &grainenv);
  }
};

