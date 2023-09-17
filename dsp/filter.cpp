#include "filter.hpp"
#include "buffer.hpp"

using namespace dspheaders;

float Comb::read(unsigned readptr) {
  // in a delay, we read at [0+n] and write at [0.f + n + offset]
  return buffer.readsample(readptr++);
}

void Comb::write(float sample) {
  buffer.writesample(sample, writeptr);
}

void Comb::write(float sample, float mod) {
  buffer.writesample(sample, writeptr + mod);
  writeptr+=1.f;
}

float Comb::play(float sample) {
  write(sample);
  return read(readptr);
}

float Comb::play(float sample, float mod) {
  write(sample, mod);
  return read(readptr);
}

Comb::Comb(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : writeptr(offset), 
    buffer(Buffer(4*samplerate, samplerate, interpolate)) {
}


