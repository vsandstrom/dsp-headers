#include "grain.hpp"
#include "dsp.h"
#include <cmath>
#include <cstdio>

using namespace dspheaders;

/*
 * ## TODO: 
 * -- Scale duration of envelope from wavetable, duration / tablelength
 * -- 
 * --
 * */


// delay travels from 0 -> 1 
float Grain::play(float delay) {
  // set function variable with position in delay g_buffer
  float out = 0.f;
  // If envelope has run its course, 'active' is set to 0 ( falsy )
  float pos = m_readptr - (delay * (*g_samplerate));
  float ptr = wrapf(&pos, g_buffer -> bufferlength);
  if (m_active) { 
    // set randomness ( jitter ) as value of seconds 
    // icrement readptr - Should mirror Granulator readptr
    m_readptr+=m_playbackrate;
    out = g_buffer->readsample(ptr) * g_envelope->play(m_dur);
    m_active = g_envelope->running();
  } else {
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
  float pos = m_readptr - (delay * g_buffer->bufferlength);
  float ptr = fmod(pos, g_buffer -> bufferlength);
  if (m_active) { 
    m_readptr+=m_playbackrate;
    // icrement readptr - Should mirror Granulator readptr
    out = g_buffer->readsample(ptr) * g_envelope->play(m_dur);
    m_active = g_envelope->running();
  } else {
    out = g_buffer->readsample(ptr) * g_envelope->play(GATE::on, m_dur);
    m_active = g_envelope->running();
  }
  return out;
}

void Grain::setDur(float dur) {
  // 512 (len) / 48000 (sr) * 0.2 (sec) ≈ 0.05333334 samples/frame
  m_dur = m_envlength / (*g_samplerate * dur) ;
}

void Grain::setRate(float rate) {
  m_playbackrate = rate;
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
