#include "grain.hpp"
#include "waveshape.h"
#include "interpolation.hpp"
#include "envelope.hpp"
#include <cstdio>
#include <iterator>

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

float Granulator::process(float position) {
  // write(sample);
  float out = 0.f;
  for (int i = 0; i < m_maxgrains; i++) {
    if (g_grains[i].m_active) {
      out += g_grains[i].play(position, m_playbackrate);
    }
  }
  return out;
}

float Granulator::process(float position, float trigger) {
  // write(sample);
  float out = 0.f;
  bool found = false;
  for (int i = 0; i < m_maxgrains; i++) {
    if (!g_grains[i].m_active && !found && trigger > 0.f) {
      // Find first free grain to activate.
      out += g_grains[i].play(position, m_playbackrate);
      found = !found;
    } else if (g_grains[i].m_active) {
      // Get sound from already active grains
      out += g_grains[i].play(position, m_playbackrate);
    }
  }
  return out;
}


float Granulator::process(float position, float rate, float trigger) {
  // write(sample);
  float out = 0.f;
  bool found = false;
  if (m_playbackrate != rate) {
    m_playbackrate = rate;
  }
  for (int i = 0; i < m_maxgrains; i++) {
    if (!g_grains[i].m_active && !found && trigger > 0) {
      // Find first free grain to activate.
      out += g_grains[i].play(position, m_playbackrate);
      found = !found;
    } else if (g_grains[i].m_active) {
      // Get sound from already active grains
      out += g_grains[i].play(position, m_playbackrate);
    }
  }
  return out;
}

// Ctor

// Default envelope - works fine to get going
Granulator::Granulator(
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
    g_grains[i] = Grain(0, 0.2, &g_samplerate, g_buffer, g_envelope);
  }
};

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
    
// Envelope from float array
Granulator::Granulator(
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
  g_grains = (Grain *)malloc(sizeof(Grain)*m_maxgrains);
  if (g_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {
    g_grains[i] = Grain(0, 0.2, &g_samplerate, g_buffer, g_envelope);
  }
};

    
// Envelope from envelope object
Granulator::Granulator(
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
    g_grains[i] = Grain(0, 0.2, &g_samplerate, g_buffer, g_envelope);
  }
};

// Dtor

Granulator::~Granulator(){
  free(g_grains);
}
