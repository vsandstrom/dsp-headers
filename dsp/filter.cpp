#include "filter.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include <cstdio>

using namespace dspheaders;

float Comb::read(unsigned readptr) {
  // in a delay, we read at [0+n] and write at [0.f + n + offset]
  float readsample = buffer.readsample(readptr);
  return readsample;
}

void Comb::write(float sample) {
  buffer.writesample(sample, writeptr);
}

void Comb::write(float sample, float mod) {
  buffer.writesample(sample, writeptr + mod);
}

float Comb::play(float sample, float feedback) {
  float dly = read(readptr);
  readptr++; 
  float out = sample + (dly * feedback);
  float output = dcblock(out, previn, prevout);
  previn = sample;
  prevout = out;
  write(out);
  writeptr+=1.f;
  return output;
}

float Comb::play(float sample, float feedback, float mod) {
  float dly = read(readptr);
  readptr++; 
  float out = sample + (dly * feedback);
  float output = dcblock(out, previn, prevout);
  previn = sample;
  prevout = out;
  write(out, mod);
  writeptr+=1.f;
  return output;
}

Comb::Comb(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : writeptr(offset), 
    buffer(Buffer(4*samplerate, samplerate, interpolate)) {
}


