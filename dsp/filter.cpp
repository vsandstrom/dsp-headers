#include "filter.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include "interpolation.hpp"
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

float Comb::play(float sample, float feedback) {
  float output = 0.f;
  float dly = read(readptr);
  readptr+=1.f; 
  float out = sample + (dly * feedback);
  output = interpolation::slope(prevout, out);
  output = dcblock(output, previn, prevout);
  previn = sample;
  prevout = out;
  write(out);
  writeptr++;
  return output;
}

float Comb::play(float sample, float feedback, float mod) {
  float output = 0.f;
  float dly = read(readptr);
  // readptr reads dragging behind writeptr

  float out = sample + (dly * feedback);

  output = interpolation::slope(prevout, out);
  // dcblock
  output = dcblock(output, previn, prevout);

  // store previous in and out
  previn = sample;
  prevout = out;

  write(out);
  readptr += 1.f + mod; 
  writeptr++;
  return output;
}

Comb::Comb(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : readptr((4*samplerate) - offset), 
    buffer(Buffer(4*samplerate, samplerate, interpolate)) {
}


