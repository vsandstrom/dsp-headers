#include "grain.hpp"
#include "dsp.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace dspheaders;

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
  // float pos = m_readptr - (position * (*g_samplerate) + m_random);
  m_random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * m_jitter;
  if (m_active) { 
    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->play(m_dur);
    // inc
    m_readptr+=m_playbackrate;
    m_active = g_envelope->running();
  } else {
    // initializes readpointer to a new start position
    m_readptr = ((position + m_random) * g_buffer -> bufferlength);
    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->play(GATE::on, m_dur);
    m_readptr+=m_playbackrate;
    m_active = g_envelope->running();
  }
  return out;
}

// delay travels from 0 -> 1 
float Grain::play(float position, float rate) {
  setRate(rate);
  float out = 0.f;
  m_random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * m_jitter;
  if (m_active) { 
    // icrement readptr - Should mirror Granulator readptr
    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->play(m_dur);
    m_readptr+=m_playbackrate;
    m_active = g_envelope->running();
  } else {
    // initializes readpointer to a new start position
    m_readptr = ((position + m_random) * g_buffer -> bufferlength);
    out = g_buffer->readsample(m_readptr);
    out *= g_envelope->play(GATE::on, m_dur);
    m_readptr+=m_playbackrate;
    m_active = g_envelope->running();
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
