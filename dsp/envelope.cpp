#include "envelope.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include "wavetable.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>


// TODO: Perhaps save segments in separate buffers, making it easier to set duration of attack,
// decay etc.

using namespace dspheaders;
// float [points], unsigned pLen, float [times], unsigned tLen, unsigned
// samplerate, interpolation-callback
Envelope::Envelope(
    float* points, unsigned pLen,
    float* times, unsigned tLen,
    float samplerate,
    float (*interpolate)(float, float*, unsigned)) 
  : points(points), 
    times(times),
    pLen(pLen),
    tLen(tLen),
    buffer(
        Buffer(
          sum(times, tLen), samplerate, interpolate
        )
      ), 
    samplerate(samplerate) {
  generate();
};

Envelope::Envelope(
    float* points, unsigned pLen,
    float* times, unsigned tLen,
    float* curves, unsigned cLen,
    float samplerate,
    float (*interpolate)(float, float*, unsigned)) 
  : points(points), 
    times(times),
    curves(curves),
    pLen(pLen),
    tLen(tLen),
    cLen(cLen),
    buffer(
        Buffer(
          sum(times, tLen), samplerate, interpolate
        )
      ), 
    samplerate(samplerate) {
  generateCurve();
};

void Envelope::generate() {
  unsigned pos = 0;
  float min = 0.f, max = 0.f;

  for (unsigned i = 0; i < pLen-1; i++) {
    // works because points need to be only positive. 
    // needs testing at construction
    max = points[i] >= points[i+1] ? points[i] : points[i+1];
    min = points[i] >= points[i+1] ? points[i+1] : points[i];

    float time = times[i];
    float numsamples = time * samplerate;
    float inc = (max - min) / numsamples;
    for (unsigned j = 0; j < (int)numsamples; j++) {
      // write function to apply curve on envelope segment here.
      float slope = j * inc;
      if (points[i] > points[i+1]) {
        buffer.buffer[pos] = points[i] - slope;
        pos++;
      } else if (points[i] <= points[i+1]){
        buffer.buffer[pos] = points[i] + slope;
        pos++;
      }
    }
  }
  readptr = pos;
  
};

void Envelope::generateCurve() {
  unsigned pos = 0;
  float q = 0.f;

  for (unsigned i = 0; i < pLen - 1; i++) {
    q = fabs(points[i+1] - points[i]);
    int numsamples = (int)(times[i] * samplerate);
    float m = 1.f / (numsamples);

    for (unsigned j = 0; j < numsamples; j++) {
      float slope = q * powf(m * j, curves[i]);

      if (points[i] >= points[i+1]) {
        buffer.buffer[pos] = points[i] - slope;
        pos++;
      } else {
        buffer.buffer[pos] = points[i] + slope;
        pos++;
      }
    }
  }
  readptr = pos+1;

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
    out = buffer.readsample(readptr);
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
    out = buffer.readsample(readptr);
  }
  readptr += speed;
  return out;
};

void Envelope::repr() {
  for (int i = 0; i < buffer.bufferlength; i++) {
    std::cout << buffer.buffer[i] << '\n';
  }
}
      

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
