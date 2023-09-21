#include "filter.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include "interpolation.hpp"
#include <cstdio>

using namespace dspheaders::filter;

//////////////////////////////////////////////////////////////////////////////
/// COMB FEEDBACK
//////////////////////////////////////////////////////////////////////////////

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
  // readptr reads dragging behind writeptr
  float out = sample - (dly * feedback);
  // output = interpolation::slope(prevout, out);
  //dcblock
  output = dcblock(output, previn, prevout);

  previn = sample;
  prevout = out;

  write(out);
  readptr+=1.f; 
  writeptr++;
  return output;
}

float Comb::play(float sample, float feedback, float mod) {
  float output = 0.f;
  float dly = read(readptr);
  // readptr reads dragging behind writeptr
  float out = sample - (dly * feedback);

  // when modulating the readptr, we need to do
  // a simple "interpolation", using the mean value
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

// Naming wrapper around Comb class, which is as default a IIR Comb filter
CombIIR::CombIIR(
  unsigned offset,
  unsigned samplerate,
  float (*interpolate)(float, float*, unsigned)) 
  : Comb(offset, samplerate, interpolate) {
};

//////////////////////////////////////////////////////////////////////////////
/// ALLPASS 
//////////////////////////////////////////////////////////////////////////////

Allpass::Allpass(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : 
    Comb(offset, samplerate, interpolate)
{}

float Allpass::play(float sample, float feedback) {
  float output = 0.f;
  float dly = read(readptr);
  // readptr reads dragging behind writeptr
  float out = sample - (dly * feedback);
  // output = interpolation::slope(prevout, out);
  //dcblock
  output = dcblock(output, previn, prevout);

  previn = sample;
  prevout = out;

  write(out);
  readptr+=1.f; 
  writeptr++;
  return output;
}


float Allpass::play(float sample, float feedback, float mod) {
  float output = 0.f;
  float dly = read(readptr);
  // readptr reads dragging behind writeptr
  float out = sample - (dly * feedback);

  // when modulating the readptr, we need to do
  // a simple "interpolation", using the mean value
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
      
CombFIR::CombFIR (
  unsigned offset,
  unsigned samplerate,
  float (*interpolate)(float, float*, unsigned))
  : Comb(offset, samplerate, interpolate) {
};

float CombFIR::play (float sample, float wet) {
  float output = 0.f;

  // write input first
  write(sample);
  // read later
  float dly = read(readptr);
  // Combine input and read buffer sample
  output = sample + (dly * wet);
  // Slope 
  // output = interpolation::slope(prevout, out);
  // dcblock
  // output = dcblock(output, previn, prevout);

  // previn = sample;
  // prevout = output;

  readptr+=1.f; 
  writeptr++;
  return output;
}

float CombFIR::play (float sample, float wet, float mod) {
  float output = 0.f;
  // write input first
  write(sample);
  // read delay
  float dly = read(readptr);

  float out = sample + (dly * wet);

  // when modulating the readptr, we need to do
  // a simple "interpolation", using the mean value
  output = interpolation::slope(prevout, out);
  // dcblock
  output = dcblock(output, previn, prevout);

  // store previous in and out
  previn = sample;
  prevout = out;

  readptr += 1.f + mod; 
  writeptr++;
  return output;
}
