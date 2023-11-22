#include "grain.hpp"
#include <cmath>

using namespace dspheaders;
/*
 * ## TODO: 
 * -- Scale duration of envelope from wavetable, duration / tablelength
 * -- 
 * --
 * */

// readposition travels from 0 -> 1 
float Grain::play(float readposition) {
  // set function variable with position in delay buffer
  float out = 0.f;
  // If envelope has run its course, 'active' is set to 0 ( falsy )
  if (active) {
    float ptr = fmod(
      readptr + (unsigned)(readposition * buffer->bufferlength), 
      buffer -> bufferlength
    );
    readptr+=1;
    // icrement readptr - Should mirror Granulator readptr
    out = buffer->readsample(ptr)*envelope->play(graindur);
    active = envelope->running();
  }
  return out;
}

void Grain::setDur(float dur) {
  graindur = (envlength / dur) * samplerate;
}

Grain::Grain(
  float readptr,
  float graindur,
  float samplerate,
  Buffer* buffer,
  Envelope* envelope) 
  : readptr(readptr),
    buffer(buffer), 
    graindur(graindur),
    envelope(envelope), 
    samplerate(samplerate),
    envlength(envelope -> getBufferlength()) {}
