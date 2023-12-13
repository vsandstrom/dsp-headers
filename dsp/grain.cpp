#include "grain.hpp"
#include <cmath>
#include <cstdlib>

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
    // set randomness ( jitter ) as value of seconds 
    float pos = readptr + (readposition * buffer->bufferlength);
    float ptr = fmod(pos, buffer -> bufferlength);
    readptr+=playbackspeed;
    // icrement readptr - Should mirror Granulator readptr
    out = buffer->readsample(ptr)*envelope->play(graindur);
    active = envelope->running();
  }
  return out;
}

// readposition travels from 0 -> 1 
float Grain::play(float readposition, float speed) {
  setSpeed(speed);

  // set function variable with position in delay buffer
  float out = 0.f;
  // If envelope has run its course, 'active' is set to 0 ( falsy )
  if (active) { 
    // set randomness ( jitter ) as value of seconds 
    float pos = readptr + (readposition * buffer->bufferlength);
    float ptr = fmod(pos, buffer -> bufferlength);
    readptr+=playbackspeed;
    // icrement readptr - Should mirror Granulator readptr
    out = buffer->readsample(ptr)*envelope->play(graindur);
    active = envelope->running();
  }
  return out;
}

void Grain::setDur(float dur) {
  graindur = (envlength / dur) * samplerate;
}

void Grain::setSpeed(float speed) {
  playbackspeed = speed;
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
