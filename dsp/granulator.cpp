#include "grain.hpp"
#include "waveshape.h"
#include "envelope.hpp"
#include <cstdio>

using namespace dspheaders;

/*
 * TODO:
 * Should we keep track of all active / non-active grains
 * by bitencoding a 32bit number, capping number of grains
 * to 32 - would require finessing
 *
 * There should be both time and space jitter.
 * One for providing aditional delay
 * One for providing a delay spread of the grains
 *
 * */
    
// 
// float jit = (jitter * static_cast<float>(rand()) / RAND_MAX) * samplerate;

// Accesspoint

float Granulator::process() {
  float out = 0.f;
  // sum all active grains
  for (int i = 0; i < m_numgrains; i++) {
    if (g_grains[i].m_active) {
      out += g_grains[i].play();
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
    if (g_grains[i].m_active == false) {
      newTriggered = i;
      out += g_grains[i].play(position, m_playbackrate); 
      break;
    }
    i++;
  }
  // sum other active grains
  for (int j = 0; j < m_numgrains; j++) {
    if (j != newTriggered && g_grains->m_active) {
      out += g_grains[j].play();
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
    if (g_grains[i].m_active == false) {
      newTriggered = i;
      out += g_grains[i].play(position, m_playbackrate); 
      break;
    }
    i++;
  }

  for (int j = 0; j < m_numgrains; j++) {
    if (j != newTriggered && g_grains->m_active) {
      out += g_grains[j].play();
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
  g_grains = (Grain*)malloc(sizeof(Grain) * m_maxgrains);
  if (g_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {

    g_grains[i] = Grain(0, dur, &g_samplerate, g_buffer, g_envelope);
  }
};

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
    
// Envelope from float array
Granulator::Granulator(
  float dur,
  float samplerate, 
  float* table, 
  unsigned tablelength,
  Buffer* buffer,
  unsigned maxgrains,
  float (interpolate)(float, float*, unsigned))
  : g_samplerate(samplerate),
    g_envelope(new Envelope(table, tablelength, samplerate, interpolate)),
    g_buffer(buffer),
    m_maxgrains(maxgrains) { 

  // create grains
  g_grains = (Grain *)malloc(sizeof(Grain) * m_maxgrains);
  if (g_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {

    g_grains[i] = Grain(0, dur, &g_samplerate, g_buffer, g_envelope);
  }
};

    
// Envelope from envelope object
Granulator::Granulator(
  float dur,
  float samplerate, 
  Envelope* grainEnvelope, 
  Buffer* buffer)
  : g_samplerate(samplerate),
    g_envelope(grainEnvelope),
    g_buffer(buffer)
{ 

  // allocate memory for all grains
  g_grains = (Grain *)malloc(sizeof(Grain)*m_maxgrains);
  if (g_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {
    // pass audio buffer and grain envelope by reference,
    g_grains[i] = Grain(0, dur, &g_samplerate, g_buffer, g_envelope);
  }
};

// Dtor

Granulator::~Granulator(){
  free(g_grains);
}
