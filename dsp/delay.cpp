/* ˚◊˚ */

#include "buffer.hpp"
#include "delay.hpp"
#include "dsp_math.h"
#include "interpolation.hpp"
#include <cstddef>

using namespace dspheaders;

//////////////////////////////////////////////////////////
//////////////////// CONSTRUCTORS: ///////////////////////
//////////////////////////////////////////////////////////
Delay::Delay(
    unsigned samplerate,
    float time,
    float maxtime,
    unsigned taps,
    float (*interpolate)(float, float*, size_t)
    ) : m_buffer(Buffer(find_pow_two(maxtime * samplerate), samplerate, interpolate)),
        g_samplerate(samplerate), 
        m_time(time),
        m_pos_mask(find_pow_two(maxtime * samplerate) -1),
        m_taps(taps) { }

Delay::Delay(
    unsigned samplerate,
    float time,
    float maxtime,
    float (*interpolate)(float, float*, size_t)
    ) : m_buffer(Buffer(find_pow_two(maxtime * samplerate), samplerate, interpolate)),
        g_samplerate(samplerate), 
        m_time(time),
        m_pos_mask(find_pow_two(maxtime * samplerate) -1),
        m_taps(1) { }

//////////////////////////////////////////////////////////
////////////////// INSTANCE METHODS: /////////////////////
//////////////////////////////////////////////////////////

float Delay::process(float input, float feedback) {
  float out = m_buffer.buffer[m_writeptr];
  m_buffer.buffer[m_writeptr] = 0.0f;
  for (int i = 0; i < m_taps; i++) {
    unsigned delay = (static_cast<unsigned>(m_time * g_samplerate)) * i + m_writeptr;
    while (delay >= m_buffer.bufferlength) {
      delay -= m_buffer.bufferlength;
    }
    m_buffer.buffer[delay] += (input + (out * feedback)) * (0.5 / float(i));
  }

  m_writeptr++;
  while (m_writeptr >= m_buffer.bufferlength) {
    m_writeptr -= m_buffer.bufferlength;
  }
  // m_writeptr = (m_writeptr+1) & m_pos_mask;
  return out;
}

//////////////////////////////////////////////////////////
////////////////// Read for REVERB: //////////////////////
//////////////////////////////////////////////////////////

float Delay::read(int offset) {
  float tap = (float)m_writeptr + offset;
  return m_buffer.readsample(tap);
}

void Delay::write(float sample, int offset) {
  // Within bounds-checking is handled in the Buffer object
  int write = m_writeptr + offset;
  m_buffer.writesample(sample, write);
  m_writeptr++;
  // wrap_dangerously(&m_writeptr, m_buffer.bufferlength);
}

//////////////////////////////////////////////////////////
//////////////////////   IDelay   ////////////////////////
//////////////////////////////////////////////////////////

IDelay::IDelay(
    unsigned samplerate,
    float time,
    float maxtime,
    unsigned taps, 
    float (*interpolate)(float, float*, size_t)
    ) : Delay{samplerate, time, maxtime, taps, interpolate} { }

IDelay::IDelay(
    unsigned samplerate,
    float time,
    float maxtime,
    float (*interpolate)(float, float*, size_t)
    ) : Delay{samplerate, time, maxtime, interpolate} { }


float IDelay::process(float input, float feedback) {
  float out = 0.0f;
  float delay = m_time * g_samplerate;

  for (int i = 1; i <= m_taps; i++) {
    float pos = i*delay;
    while (pos >= m_buffer.bufferlength) {
      pos -= m_buffer.bufferlength;
    }
    out += m_buffer.readsample(pos) / float(i+1);
  }

  m_buffer.buffer[m_writeptr] = input + (out * feedback);
  m_writeptr++;
  while (m_writeptr >= m_buffer.bufferlength) {
    m_writeptr -= m_buffer.bufferlength;
  }

  return out;
}

