#include "filter.hpp"
#include "buffer.hpp"
#include "dsp.h"

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
  float fb = read(readptr);
  readptr++; 
  write(sample + (fb * feedback));
  writeptr+=1.f;
  return sample + fb;
}

float Comb::play(float sample, float feedback, float mod) {
  float fb = read(readptr);
  readptr++; 
  write(sample + (fb * feedback), mod);
  writeptr+=1.f;
  return sample + fb;
}

Comb::Comb(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : writeptr(offset), 
    buffer(Buffer(4*samplerate, samplerate, interpolate)) {
}


