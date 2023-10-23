#include "envelope.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include "wavetable.hpp"
#include <algorithm>
#include <cstdio>


// TODO: Perhaps save segments in separate buffers, making it easier to set duration of attack,
// decay etc.

using namespace dspheaders;
// float [breakpoints], unsigned pointlength, float [breaktimes], unsigned timeslength, unsigned
// samplerate, interpolation-callback
Envelope::Envelope(
    float* breakpoints, unsigned pointlength,
    float* breaktimes, unsigned timeslength,
    float samplerate,
    float (*interpolate)(float, float*, unsigned)) 
  : breakpoints(breakpoints), 
    breaktimes(breaktimes),
    pointlength(pointlength),
    timeslength(timeslength),
    buffer(
        Buffer(
          sum(breaktimes, timeslength), samplerate, interpolate
        )
      ), 
    samplerate(samplerate) {
  generate();
};

void Envelope::generate() {
  unsigned pos = 0;
  float min = 0.f, max = 0.f;

  for (unsigned i = 0; i < pointlength-1; i++) {
    // works because breakpoints need to be only positive. 
    // needs testing at construction
    max = breakpoints[i] >= breakpoints[i+1] ? breakpoints[i] : breakpoints[i+1];
    min = breakpoints[i] >= breakpoints[i+1] ? breakpoints[i+1] : breakpoints[i];


    float time = breaktimes[i];
    float numsamples = time * samplerate;
    float inc = (max - min) / numsamples;
    for (unsigned j = 0; j < (int)numsamples; j++) {
      // write function to apply curve on envelope segment here.
      float slope = j * inc;
      if (breakpoints[i] > breakpoints[i+1]) {
        buffer.buffer[pos] = breakpoints[i] - slope;
        pos++;
      } else if (breakpoints[i] <= breakpoints[i+1]){
        buffer.buffer[pos] = breakpoints[i] + slope;
        pos++;
      }
    }
  }
  readptr = pos;
  
};

// Returns current value from table
// float Envelope::play() {
//   float out = 0.f;
//   if (readptr < buffer.bufferlength) {
//     out = buffer.readsample(wrapf(&readptr, buffer.bufferlength));
//     readptr += 1.f;
//   } 
//   return out;
// }

// Resets envelope to start and returns the first value from table
float Envelope::play(GATE trigger) {
  float out = 0.f;
  if (trigger == GATE::off) {
    if (readptr < buffer.bufferlength) {
      out = buffer.readsample(readptr);
      prev = out;
    }
  } else if (trigger == GATE::on) {
    readptr = 0.f;
    // Small smoothing step 
    out = (buffer.readsample(readptr) + prev) / 2;
  }
  readptr += 1.f;
  return out;
};

float Envelope::play(GATE trigger, float speed) {
  float out = 0.f;
  if (trigger == GATE::off) {
    if (readptr < buffer.bufferlength) {
      out = buffer.readsample(readptr);
      prev = out;
    }
  } else if (trigger == GATE::on) {
    readptr = 0.f;
    // Small smoothing step 
    out = (buffer.readsample(readptr) + prev) / 2;
  }
  readptr += speed;
  return out;
};
      

PercEnv::PercEnv(float attack, float decay, float samplerate, float (*interpolate)(float, float*, unsigned))
  : attack(attack), decay(decay), samplerate(samplerate), buffer(Buffer(attack+decay, samplerate, interpolate)) {
  generate();
}

void PercEnv::generate() {
  unsigned atime = (attack * samplerate) + 0.5;
  unsigned dtime = (decay * samplerate) + 0.5f;

  float ainc = 1.f / (float)atime;
  float dinc = 1.f / (float)dtime;
  
  for (unsigned i = 0; i < atime; i++) {
    buffer.buffer[i] = ainc * i;
  }
  for (unsigned i = 0; i < (atime + dtime) -1 ; i++) {
    buffer.buffer[atime + i] = 1 - (dinc*i);
  }
}

float PercEnv::play() {
  float out = 0.f;
  if (readptr < buffer.bufferlength) {
     out = buffer.readsample(wrapf(&readptr, buffer.bufferlength));
    readptr += 1.f;
  } 
  return out;
}

float PercEnv::play(GATE trigger) {
  float out = 0.f;
  if (trigger == GATE::off) {
    if (readptr < buffer.bufferlength) {
    out = buffer.readsample(wrapf(&readptr, buffer.bufferlength));
      readptr += 1.f;
    } 
    return out;
  }
  readptr = 0.f;
  out = buffer.readsample(wrapf(&readptr, buffer.bufferlength));
  readptr += 1.f;
  return out;
}
