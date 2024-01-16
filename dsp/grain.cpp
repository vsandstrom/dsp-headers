#include "grain.hpp"
#include "dsp.h"
#include <cstdio>
#include <cstdlib>
#include <memory>


using namespace dspheaders;

static int x = 0;

/*
 * TODO: 
 * Scale duration of envelope from wavetable, duration / tablelength
 */

// delay travels from 0 -> 1 

// Play-method for already triggered grain
float Grain::play() {
  float out = 0.f;
  out = g_buffer->readsample(m_readptr);
  out *= g_envelope->read(m_envptr);

  m_readptr+=m_playbackrate;
  m_envptr += m_dur;

  if (m_envptr > g_envelope->getBufferlength()) { m_active = false; }
  return out;
}

// Play-method for new grains
float Grain::play(float position, float rate) {
  float out = 0.f;
  m_playbackrate = rate;
  // initializes readpointer to a new start position
  m_random = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * m_jitter;
  m_readptr = ((position + m_random) * g_buffer -> bufferlength);
  m_envptr = 0.f;

  out = g_buffer->readsample(m_readptr);
  out *= g_envelope->read(m_envptr);

  m_readptr+=m_playbackrate;
  m_envptr+=m_dur;
  m_active = true;
  return out;
}

Grain::Grain(
  float readptr,
  float dur,
  float* samplerate,
  std::shared_ptr<Buffer> buffer,
  std::shared_ptr<Envelope> envelope) 

  : g_buffer(buffer), 
    g_envelope(envelope), 
    g_samplerate(samplerate),
    m_envlength(envelope -> getBufferlength()),
    m_readptr(readptr)
    { setDur(dur);
}

Grain::Grain(){}


void Grain::test() {
  printf("ALLOCATED");
}
