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
float Grain::play(float delay) {
  float out = 0.f;
  float pos = m_readptr - (delay * (*g_samplerate) + m_random);
  float ptr = wrapf(&pos, g_buffer -> bufferlength);
  if (m_active) { 
    m_readptr+=m_playbackrate;
    out = g_buffer->readsample(ptr) * g_envelope->play(m_dur);
    m_active = g_envelope->running();
  } else {
    m_random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * m_jitter;
    out = g_buffer->readsample(ptr) * g_envelope->play(GATE::on, m_dur);
    m_active = g_envelope->running();

  }
  return out;
}

// delay travels from 0 -> 1 
float Grain::play(float delay, float rate) {
  setRate(rate);

  // set function variable with position in delay g_buffer
  float out = 0.f;
    
  // If envelope has run its course, 'active' is set to 0 ( falsy )
  float pos = m_readptr - (delay * g_buffer->bufferlength + m_random);
  float ptr = fmod(pos, g_buffer -> bufferlength);
  if (m_active) { 
    m_readptr+=m_playbackrate;
    // icrement readptr - Should mirror Granulator readptr
    out = g_buffer->readsample(ptr) * g_envelope->play(m_dur);
    m_active = g_envelope->running();
  } else {
    m_random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * m_jitter;
    out = g_buffer->readsample(ptr) * g_envelope->play(GATE::on, m_dur);
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
