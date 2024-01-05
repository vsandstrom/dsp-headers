#include "filter.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include "interpolation.hpp"

/// Diagrams lifted from CCRMAs webpage
/// https://ccrma.stanford.edu/~jos/pasp/Feedforward_Comb_Filters.html
/// https://ccrma.stanford.edu/~jos/pasp/Feedback_Comb_Filters.html
/// https://ccrma.stanford.edu/~jos/pasp/Allpass_Two_Combs.html
using namespace dspheaders;

float Comb::read(float readptr) {
  // in a delay, we read at [0+n] and write at [0.f + n + offset]
  float readsample = buffer.readsample(readptr);
  return readsample;
}

void Comb::write(float sample) {
  buffer.writesample(sample, writeptr);
}

float Comb::iir(float sample, float feedback) {
  ///
  ///                 feedback comb filter
  ///
  ///                ╓─────────────────> y(n)
  ///                ║   ╓─────────╖ 
  ///  x(n) ─> ( + )─╨─> ║  z(-M)  ║──╖
  ///            Λ       ╙─────────╜  ║ 
  ///            ╙────────( * aM ) <──╜
  ///
  float output = 0.f;
  // read buffer & write buffer with curr sample + dly value, causing feedback
  float out = sample + (read(readptr) * feedback);
  write(out);
  return out;
}

float Comb::iir(float sample, float feedback, float mod) {
  ///
  ///                 feedback comb filter
  ///
  ///               ╓─────────────────> y(n)
  ///               ║   ╓─────────╖ 
  ///  x(n) ─>( + )─╨─> ║  z(-M)  ║──╖
  ///           Λ       ╙─────────╜  ║ 
  ///           ╙────────( * aM ) <──╜
  ///
  float output = 0.f;
  float dly = read(readptr + mod);
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
  return output;
}


float Comb::fir(float sample, float amp) {
  ///
  ///         feedforward comb filter
  ///
  ///        ╓───> ( * b0 )────────╖
  ///        ║   ╓─────────╖       V
  ///  x(n) ─╨─> ║  z(-M)  ║───> ( + )──> y(n)
  ///            ╙─────────╜    
  ///
  float output = 0.f;
  // write only current sample to buffer, not causing feedback
  write(sample);
  return sample + (read(readptr) * amp);
}

float Comb::fir(float sample, float amp, float mod) {
  ///
  ///        feedforward comb filter
  ///
  ///        ╓───> ( * b0 )────────╖
  ///        ║   ╓─────────╖       V
  ///  x(n) ─╨─> ║  z(-M)  ║ ──> ( + )──> y(n)
  ///            ╙─────────╜    
  ///
  float output = 0.f;
  float dly = read(readptr + mod);
  write(sample);
  // readptr reads dragging behind writeptr
  float out = sample + (dly * amp);
  return out;
}

float Comb::process(float sample, float feedback, COMBTYPE type) {
  float output = 0.f;
  switch (type) {
    case IIR : { output = iir(sample, feedback); break; };
    case FIR : { output = fir(sample, feedback); break; };
    default: { break; };
  }
  readptr += 1.f; 
  writeptr++;
  return output;
}

float Comb::process(float sample, float feedback, float mod, COMBTYPE type) {
  float output = 0.f;
  switch (type) {
    // "mod" arg not used inside function "iir/fir" function any more 
    // only used to distinguish between nonslope and slope versions
    // of the "iir"-function
    case IIR : { output = iir(sample, feedback + mod); break; };
    case FIR : { output = fir(sample, feedback + mod); break; };
    default: { break; };
  }
  readptr += 1.f; 
  writeptr++;
  return output;
}

Comb::Comb(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : buffer(Buffer(4*samplerate, samplerate, interpolate)),
    readptr((float)(buffer.bufferlength - offset)) { }


Allpass::Allpass(
  unsigned offset,
  unsigned samplerate,
  float (*interpolate)(float, float*, unsigned))
  : Comb(offset, samplerate, interpolate) {
}

// float sample - current input
//
// float coeff - the feedback and feedforward of internal comb filters
float Allpass::process(float sample, float coeff) {
  ///
  ///                 allpass filter
  ///
  ///                ╓───> ( * b0 )─────────╖
  ///                ║   ╓─────────╖        V
  ///  x(n) ─> ( + )─╨─> ║  z(-M)  ║──╥─> ( + )──> y(n)
  ///            Λ       ╙─────────╜  ║ 
  ///            ╙────────( * -aM ) <─╜
  ///
  ///       where: b0 == aM

  float bck = iir(sample, -coeff);
  float fwd = fir(sample + bck, coeff);
  float out = interpolation::slope(fwd, prevout);
  prevout = out;
  readptr+=1.f; 
  writeptr++;
  return out;
}

// float sample - current input
//
// float coeff - the feedback and feedforward of internal comb filters
//
// float mod - modulates the read head of the buffer
float Allpass::process(float sample, float coeff, float mod) {
  float bck = iir(sample, -coeff, mod);
  float fwd = fir(sample + bck, coeff, mod);
  float out = interpolation::slope(fwd, prevout);
  prevout = out;
  readptr+=1.f; 
  writeptr++;
  return out;
}
