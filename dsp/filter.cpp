#include "filter.hpp"
#include "buffer.hpp"
#include "dsp.h"
#include "interpolation.hpp"
#include <cstdio>

/// Diagrams lifted from CCRMAs webpage
/// https://ccrma.stanford.edu/~jos/pasp/Feedforward_Comb_Filters.html
/// https://ccrma.stanford.edu/~jos/pasp/Feedback_Comb_Filters.html
/// https://ccrma.stanford.edu/~jos/pasp/Allpass_Two_Combs.html
using namespace dspheaders;

float Comb::m_read() {
  // in a delay, we read at [0+n] and write at [0.f + n + offset]
  float readsample = m_buffer.readsample((float)m_readptr);
  return readsample;
}

void Comb::m_write(float sample) {
  m_buffer.writesample(sample, (int)m_writeptr);
}
  
///
///                 feedback comb filter
///
///                ╓─────────────────> y(n)
///                ║   ╓─────────╖ 
///  x(n) ─> ( + )─╨─> ║  z(-M)  ║──╖
///            Λ       ╙─────────╜  ║ 
///            ╙────────( * aM ) <──╜
///

float Comb::m_feedback(float sample, float feedback) {
  float output = 0.f;
  // read buffer & write buffer with curr sample + dly value, causing feedback
  float out = sample + (m_read() * feedback);
  m_write(out);
  return out;
}

// float Comb::m_feedback(float sample, float feedback) {
//   float output = 0.f;
//   float dly = m_read();
//   // readptr reads dragging behind writeptr
//   float out = sample + (dly * feedback);
//
//   // when modulating the readptr, we need to do
//   // a simple "interpolation", using the mean value
//   output = interpolation::slope(prevout, out);
//   output = dcblock(output, previn, prevout);
//   // store previous in and out
//   previn = sample;
//   prevout = out;
//
//   m_write(out);
//   return output;
// }

///
///         feedforward comb filter
///
///        ╓───> ( * b0 )────────╖
///        ║   ╓─────────╖       V
///  x(n) ─╨─> ║  z(-M)  ║───> ( + )──> y(n)
///            ╙─────────╜    
///

float Comb::m_feedforward(float sample, float amp) {
  float output = 0.f;
  // write only current sample to buffer, not causing feedback
  output = m_read() * amp;
  m_write(sample);
  return output;
    // sample + (m_read() * amp);
}

// float Comb::m_feedforward(float sample, float amp) {
//   float output = 0.f;
//   float dly = m_read();
//   m_write(sample);
//   // readptr reads dragging behind writeptr
//   float out = sample + (dly * amp);
//   return out;
// }

float Comb::process(float sample, float feedback, COMBTYPE type) {
  float output = 0.f;
  switch (type) {
    case IIR : { output = m_feedback(sample, feedback); break; };
    case FIR : { output = m_feedforward(sample, feedback); break; };
    default: { break; };
  }
  m_readptr+=1.f; 
  m_writeptr++;
  while (m_readptr >= m_buffer.bufferlength) m_readptr-=m_buffer.bufferlength;
  while (m_writeptr >= m_buffer.bufferlength) m_writeptr-=m_buffer.bufferlength;
  return output;
}

float Comb::process(float sample, float feedback, float mod, COMBTYPE type) {
  float output = 0.f;
  switch (type) {
    // "mod" arg not used inside function "feedback/feedforward" function any more 
    // only used to distinguish between nonslope and slope versions
    // of the "feedback"-function
    case IIR : { output = m_feedback(sample, feedback + mod); break; };
    case FIR : { output = m_feedforward(sample, feedback + mod); break; };
    default: { break; };
  }
  m_readptr += 1.f + mod; 
  m_writeptr++;
  while (m_readptr >= m_buffer.bufferlength) m_readptr-=m_buffer.bufferlength;
  while (m_writeptr >= m_buffer.bufferlength) m_writeptr-=m_buffer.bufferlength;
  return output;
}


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

// float sample - current input
//
// float coeff - the feedback and feedforward of internal comb filters
float Allpass::process(float sample, float coeff) {

  float bck = m_feedback(sample, -coeff);
  float fwd = m_feedforward(sample, coeff);
  // float out = interpolation::slope(fwd + bck, prevout);
  // prevout = out;
  m_readptr++; 
  m_writeptr++;
  while (m_readptr >= m_buffer.bufferlength) m_readptr-=m_buffer.bufferlength;
  while (m_writeptr >= m_buffer.bufferlength) m_writeptr-=m_buffer.bufferlength;
  return bck + fwd;
}

// float sample - current input
//
// float coeff - the feedback and feedforward of internal comb filters
//
// float mod - modulates the read head of the buffer
float Allpass::process(float sample, float coeff, float mod) {
  float bck = m_feedback(sample, -coeff);
  float fwd = m_feedforward(sample, coeff);
  // float out = interpolation::slope(fwd+bck, prevout);
  // prevout = out;
  m_readptr += 1.f + mod; 
  m_writeptr++;
  while (m_readptr >= m_buffer.bufferlength) m_readptr-=m_buffer.bufferlength;
  while (m_writeptr >= m_buffer.bufferlength) m_writeptr-=m_buffer.bufferlength;
  return bck + fwd;
}

Comb::Comb(
    unsigned offset,
    unsigned samplerate,
    float (*interpolate)(float, float*, unsigned))
  : m_buffer(offset, samplerate, interpolate) { 
      printf("Comb - offset: %i\n", offset); 
      printf("Comb - buflen: %i\n", offset); 
    }


Allpass::Allpass(
  unsigned offset,
  unsigned samplerate,
  float (*interpolate)(float, float*, unsigned))
  : Comb(offset, samplerate, interpolate) {
}
