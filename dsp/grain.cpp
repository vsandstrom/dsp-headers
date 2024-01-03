#include "grain.hpp"
#include "dsp.h"
#include <cstdio>
#include <cstdlib>

using namespace dspheaders;

static int x = 0;

/*
 * ## TODO: 
 * -- Scale duration of envelope from wavetable, duration / tablelength
 * -- 
 * --
 * */

// THE READPOINTER FOR EACH GRAIN IS DETACHED FROM THE GRANULATORS WRITEPOINTER
// THIS MIGHT BE MESSY

// delay travels from 0 -> 1 
float Grain::play(float position) {
  float out = 0.f;
  if (m_active) { 
    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->read(m_envptr);

    m_readptr+=m_playbackrate;
    m_envptr += m_dur;

    if (m_envptr > g_envelope->getBufferlength()) { m_active = false; }

  } else {
    // initializes readpointer to a new start position
    printf("spawn grain\n");
    m_envptr = 0.f;
    m_random = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * m_jitter;
    m_readptr = ((position + m_random) * g_buffer -> bufferlength);

    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->read(m_envptr);

    m_readptr += m_playbackrate;
    m_envptr += m_dur;
    m_active = true;
  }
  return out;
}

// delay travels from 0 -> 1 
float Grain::play(float position, float rate) {
  float out = 0.f;

  m_playbackrate = rate;
  if (m_active) { 
    // icrement readptr - Should mirror Granulator readptr
    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->read(m_envptr);

    m_readptr+=m_playbackrate;
    m_envptr+=m_dur;

    if (m_envptr > g_envelope->getBufferlength()) { m_active = false; }

  } else {
    printf("spawn grain\n");
    // initializes readpointer to a new start position
    m_random = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * m_jitter;
    m_readptr = ((position + m_random) * g_buffer -> bufferlength);
    m_envptr = 0.f;

    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->read(m_envptr);

    m_readptr+=m_playbackrate;
    m_envptr+=m_dur;
    m_active = true;
  }
  return out;
}

Grain::Grain(
  float readptr,
  float dur,
  float* samplerate,
  Buffer* buffer,
  Envelope* envelope) 
  : g_buffer(buffer), 
    g_envelope(envelope), 
    g_samplerate(samplerate),
    m_envlength(envelope -> getBufferlength()),
    m_readptr(readptr),
    m_dur(m_envlength / (*g_samplerate * dur)) {
}


void Grain::test() {
  printf("ALLOCATED");
}
