
/* ˚◊˚ */

#include "buffer.hpp"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;

//////////////////////////////////////////////////////////
//////////////////// CONSTRUCTORS: ///////////////////////
//////////////////////////////////////////////////////////

Delay::Delay(
    unsigned samplerate,
    float time,
    float maxtime,
    unsigned taps,
    float (*interpolate)(float, float*, unsigned)
    ) : m_buffer(Buffer(maxtime, samplerate, interpolate)),
        g_samplerate(samplerate), 
        m_time(time),
        m_pos_mask(m_buffer.bufferlength -1),
        m_taps(taps) { }

Delay::Delay(
    unsigned samplerate,
    float time,
    float maxtime,
    float (*interpolate)(float, float*, unsigned)
    ) : m_buffer(Buffer(maxtime, samplerate, interpolate)), 
        g_samplerate(samplerate), 
        m_time(time),
        m_pos_mask(m_buffer.bufferlength -1),
        m_taps(1) { }

//////////////////////////////////////////////////////////
////////////////// INSTANCE METHODS: /////////////////////
//////////////////////////////////////////////////////////

float Delay::read(float delaytime, float damp) {
  float output = 0.f;
  float taptime = (delaytime * g_samplerate);
  for (unsigned i = 1; i <= m_taps; i++) {
    float tap = (float)m_writeptr - (taptime*i);
    output += m_buffer.readsample(tap) * damp;
    // damp*=damp;
    damp /= 2;
  }
  return output;
}

void Delay::write(float sample) {
  // Within bounds-checking is handled in the Buffer object
  m_buffer.writesample(sample, (int)m_writeptr++);
  // wrap_dangerously(&m_writeptr, m_buffer.bufferlength);
}

// float Delay::process(float input, float time, float wet, float feedback) {
//   float output = read(time, feedback);
//   // write the time back to write head with feedback
//   // MAGIC NUMBER for scaling the feedback of the delay to something managable
//   output = interpolation::slope((input + output), m_prev);
//   write(output);
//   m_prev = output;
//   // wet controls dry/wet balance of time
//   return output * wet;
// }

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
  m_writeptr = (m_writeptr+1) & m_pos_mask;



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
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


