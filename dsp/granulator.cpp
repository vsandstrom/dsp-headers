#include "grain.hpp"
#include "waveshape.h"
#include "envelope.hpp"
#include <cstdio>

using namespace dspheaders;

float Granulator::process() {
  float out = 0.f;
  // sum all active grains
  for (int i = 0; i < m_numgrains; i++) {
    if (m_grains[i].m_active) {
      out += m_grains[i].play();
    }
  }
  return out;
}

float Granulator::process(float position, float trigger) {
  // find available grain to trigger
  if (trigger < 1.f) { return process(); } 

  float out = 0.f;
  int i = 0;
  int newTriggered = -1;
  while (i < m_numgrains) {
    if (m_grains[i].m_active == false) {
      newTriggered = i;
      out += m_grains[i].play(position, m_playbackrate); 
      break;
    }
    i++;
  }
  // sum other active grains
  for (int j = 0; j < m_numgrains; j++) {
    if (j != newTriggered && m_grains->m_active) {
      out += m_grains[j].play();
    }
  }
  return out;
}


float Granulator::process(float position, float rate, float trigger) {
  if (m_playbackrate != rate) { m_playbackrate = rate; }
  if (trigger < 1.f) { return process(); }

  float out = 0.f;
  int i = 0;
  int newTriggered = -1;
  while (i < m_numgrains) {
    if (m_grains[i].m_active == false) {
      newTriggered = i;
      out += m_grains[i].play(position, m_playbackrate); 
      break;
    }
    i++;
  }

  for (int j = 0; j < m_numgrains; j++) {
    if (j != newTriggered && m_grains->m_active) {
      out += m_grains[j].play();
    }
  }
  return out;
}

// Ctor
// Default envelope - works fine to get going
Granulator::Granulator(
  float dur,
  float samplerate, 
  unsigned maxgrains,
  Buffer* buffer,
  float (*interpolate)(float, float*, unsigned))
  : g_samplerate(samplerate),
    g_buffer(buffer),
    m_maxgrains(maxgrains) {

  // create default grain envelope
  float* env = new float[512];
  hanning(env, 512);
  g_envelope = new Envelope(env, 512, g_samplerate, interpolate);

  // create grains
  m_grains = (Grain*)malloc(sizeof(Grain) * m_maxgrains);
  if (m_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {

    m_grains[i] = Grain(0, dur, &g_samplerate, g_buffer, g_envelope);
  }
};

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
    
// Envelope from float array
Granulator::Granulator(
  float dur,
  float samplerate, 
  unsigned maxgrains,
  float* table, 
  unsigned tablelength,
  float (interpolate)(float, float*, unsigned),
  Buffer* buffer)
  : g_samplerate(samplerate),
    g_envelope(new Envelope(table, tablelength, samplerate, interpolate)),
    g_buffer(buffer),
    m_maxgrains(maxgrains) { 

  // create grains
  m_grains = (Grain *)malloc(sizeof(Grain) * m_maxgrains);
  if (m_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {

    m_grains[i] = Grain(0, dur, &g_samplerate, g_buffer, g_envelope);
  }
};

    
// Envelope from envelope object
Granulator::Granulator(
  float dur,
  float samplerate, 
  unsigned maxgrains,
  Envelope* grainEnvelope, 
  Buffer* buffer)
  : g_samplerate(samplerate),
    g_envelope(grainEnvelope),
    g_buffer(buffer),
    m_maxgrains(maxgrains)
{ 

  // allocate memory for all grains
  m_grains = (Grain *)malloc(sizeof(Grain)*m_maxgrains);
  if (m_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {
    // pass audio buffer and grain envelope by reference,
    m_grains[i] = Grain(0, dur, &g_samplerate, g_buffer, g_envelope);
  }
};

// Dtor

Granulator::~Granulator(){
  free(m_grains);
}
