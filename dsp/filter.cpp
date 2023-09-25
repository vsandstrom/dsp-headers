#include "filter.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include "interpolation.hpp"
#include <cstdio>

using namespace dspheaders;

//////////////////////////////////////////////////////////////////////////////
/// COMB FEEDBACK
//////////////////////////////////////////////////////////////////////////////

float Comb::read(float readptr) {
  // in a delay, we read at [0+n] and write at [0.f + n + offset]
  float readsample = buffer.readsample(readptr);
  return readsample;
}

void Comb::write(float sample) {
  buffer.writesample(sample, writeptr);
}

float Comb::playIIR(float sample, float feedback) {
  float output = 0.f;
  float dly = read(readptr);
  // readptr reads dragging behind writeptr
  float out = sample + (dly * feedback);
  // output = interpolation::slope(prevout, out);
  //dcblock
  // output = dcblock(output, previn, prevout);

  // previn = sample;
  // prevout = out;

  write(out);
  readptr+=1.f; 
  writeptr++;
  return out;
}

float Comb::playIIR(float sample, float feedback, float mod) {
  float output = 0.f;
  float dly = read(readptr);
  // readptr reads dragging behind writeptr
  float out = sample + (dly * feedback);

  // when modulating the readptr, we need to do
  // a simple "interpolation", using the mean value
  output = interpolation::slope(prevout, out);
  output = dcblock(output, previn, prevout);

  // store previous in and out
  previn = sample;
  prevout = out;

  write(out);
  readptr += 1.f + mod; 
  writeptr++;
  return output;
}


float Comb::playFIR(float sample, float amp) {
  float output = 0.f;
  float dly = read(readptr);
  write(sample);
  // readptr reads dragging behind writeptr
  float out = sample + (dly * amp);
  // output = interpolation::slope(prevout, out);
  //dcblock
  // output = dcblock(output, previn, prevout);

  // previn = sample;
  // prevout = out;

  readptr+=1.f; 
  writeptr++;
  return out;
}

float Comb::playFIR(float sample, float amp, float mod) {
  float output = 0.f;
  float dly = read(readptr);
  write(sample);
  // readptr reads dragging behind writeptr
  float out = sample + (dly * amp);

  // when modulating the readptr, we need to do
  // a simple "interpolation", using the mean value
  // output = interpolation::slope(prevout, out);
  // dcblock
  // output = dcblock(output, previn, prevout);

  // store previous in and out
  // previn = sample;
  // prevout = out;

  readptr += 1.f + mod; 
  writeptr++;
  return out;
  // TODO
  return 0.f;
}

Comb::Comb(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : buffer(Buffer(4*samplerate, samplerate, interpolate)),
    readptr((float)(buffer.bufferlength - offset)) { }

