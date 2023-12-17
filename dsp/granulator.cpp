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

float Granulator::process(float sample, float delay) {
  write(sample);

  float out = 0.f;
  for (int i = 0; i < m_maxgrains; i++) {
    // float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    // out += g_grains[i].play(delay + (m_jitter * r), m_playbackrate);
    out += g_grains[i].play(0.1, 1);
  }
  return out;
}


// Speed argument propagates to the grains. pitch
float Granulator::process(float sample, float delay, float rate) {
  write(sample);
  if (m_playbackrate != rate) {
    m_playbackrate = rate;
  }

  float out = 0.f;
  for (int i = 0; i < m_maxgrains; i++) {
    // float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    // delay ([0 - 1]) + (m_jitter * r) ([0.f - 1.f] * [0.f - 1.f]) * buffer-> bufferlength = delay
    // out += g_grains[i].play(delay + (m_jitter * r), m_playbackrate);
    out += g_grains[i].play(0.1, 1);
  }
  return out;
}

void Granulator::write(float sample) {
  g_buffer.writesample(sample, m_writeptr++);
  m_writeptr %= g_buffer.bufferlength ;
}

// Ctor

// Default envelope - works fine to get going
Granulator::Granulator(
  float samplerate, 
  unsigned maxgrains,
  float (*interpolate)(float, float*, unsigned))
  : g_samplerate(samplerate),
    g_buffer(4.f, samplerate, interpolate),
    m_maxgrains(maxgrains) {

  // create default grain envelope
  float* env = new float[512];
  hanning(env, 512);
  g_envelope = new Envelope(env, 512, g_samplerate, interpolate);

  // create grains
  // g_grains = (Grain *)malloc(sizeof(Grain)*m_maxgrains);
  g_grains = (Grain*)malloc(sizeof(Grain) * m_maxgrains);
  if (g_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {
    g_grains[i] = Grain(0, 0.2, &g_samplerate, &g_buffer, g_envelope);
  }

  for (int i = 0; i < m_maxgrains; i++) {
    g_grains[i].test();
  }
};

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
    
// Envelope from float array
Granulator::Granulator(
  float samplerate, 
  float* table, 
  unsigned tablelength,
  unsigned maxgrains,
  float (interpolate)(float, float*, unsigned))
  : g_samplerate(samplerate),
    g_envelope(new Envelope(table, tablelength, samplerate, interpolate)),
    g_buffer(4.f, samplerate, interpolate),
    m_maxgrains(maxgrains) { 

  // create grains
  g_grains = (Grain *)malloc(sizeof(Grain)*m_maxgrains);
  if (g_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {
    g_grains[i] = Grain(0, 0.2, &g_samplerate, &g_buffer, g_envelope);
  }
};

    
// Envelope from envelope object
Granulator::Granulator(float samplerate, Envelope* grainEnvelope)
  : g_samplerate(samplerate),
    g_envelope(grainEnvelope),
    g_buffer(samplerate * 4, samplerate, interpolation::linear) { 

  // allocate memory for all grains
  g_grains = (Grain *)malloc(sizeof(Grain)*m_maxgrains);
  if (g_grains == nullptr) { return;} 
  for (int i = 0; i < m_maxgrains; i++) {
    // pass audio buffer and grain envelope by reference,
    g_grains[i] = Grain(0, 0.2, &g_samplerate, &g_buffer, g_envelope);
  }
};

// Dtor

Granulator::~Granulator(){
  free(g_grains);
}
