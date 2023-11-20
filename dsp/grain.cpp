#include "grain.hpp"
#include <cmath>

/*
 * ## TODO: 
 * -- Scale duration of envelope from wavetable, duration / tablelength
 * -- 
 * --
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
    grainenv(HANNING, 512, samplerate, interpolate),
    buffer(samplerate * 4, samplerate, interpolate) {

  // allocate memory for all grains
  grains = (Grain *)malloc(sizeof(Grain)*maxgrains);
 

  // if out of memory, return
  if (grains == nullptr) { return;} 
  for (int i = 0; i < maxgrains; i++) {
    // Offset-finding function, should have random argument
    // float offset = writeptr - getoffset(0.1) * (1+jitter);
    grains[i] = Grain(0, 0.2, 0.2, samplerate, &buffer, &grainenv);
  }
};
   
    
// Predefined grain envelope in float array
Granulator::Granulator(
  float samplerate, 
  float* table, 
  unsigned tablelength,
  unsigned maxgrains,
  float (interpolate)(float, float*, unsigned))
  : samplerate(samplerate),
  // Default grain envelope
    grainenv(table, tablelength, samplerate, interpolate),
    buffer(samplerate * 4, samplerate, interpolate) { 
  grains = (Grain *)malloc(sizeof(Grain)*maxgrains);
};

Granulator::~Granulator(){
  free(grains);
}
    
    
// Predefined grain envelope in Wavetable
Granulator::Granulator(float samplerate, Wavetable grainEnvelope)
: samplerate(samplerate),
  grainenv(grainEnvelope),
  buffer(samplerate * 4, samplerate, interpolation::linear) { 
    grains = (Grain *)malloc(sizeof(Grain)*maxgrains);
};

float Grain::play(float offset) {
  // set function variable with offset.
  float ptr = fmod(readptr + offset, buffer->bufferlength);
  // icrement readptr - Should mirror Granulator readptr
  readptr += 1.f;
  // 
  return buffer->readsample(ptr)*envelope->play();
}

void Grain::setDur(float dur) {
  speed = (envlength / dur) * samplerate;
}

Grain::Grain(
  float readptr,
  float speed,
  float graindur,
  float samplerate,
  Buffer* buffer,
  Wavetable* envelope) 
  : readptr(readptr),
    buffer(buffer), 
    envelope(envelope), 
    graindur(graindur),
    samplerate(samplerate),
    envlength(envelope -> getTablelength()),
    speed((envelope -> getTablelength() / graindur) * samplerate) { }
