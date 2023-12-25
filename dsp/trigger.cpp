#include "trigger.hpp"
#include <cstdio>
#include <stdlib.h>
using namespace dspheaders;


float Impulse::play() {

  if (m_count >= (m_dur * samplerate)) {
    m_count = 0;
    return 1.f;
  }
  m_count++;
  return -0.f;
}

float Impulse::play(float dur) {
  m_dur = dur;
  if (m_count >= (m_dur * samplerate)) {
    m_count = 0;
    return 1.f;
  }
  m_count++;
  return -0.f;

}

Impulse::Impulse(float dur, float samplerate)
  : m_dur(dur), samplerate(samplerate) {
  }

//////////////////////////////////////////////

float Dust::play() {
  if (m_count >= m_dur * samplerate) {
    float rng = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.f;
    m_dur = relDur * rng;
    m_count = 0;
    printf("impulse %f\n", m_dur);
    return 1.f;
  }
  m_count++;
  return 0.f;
}

float Dust::play(float relativeDur) {
  relDur = relativeDur;
  if (m_count >= m_dur * samplerate) {
    float rng = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.f;
    m_dur = relativeDur * rng;
    m_count = 0;
    printf("impulse %f\n", m_dur);
    return 1.f;
  }
  m_count++;
  return 0.f;
}

Dust::Dust(float relativeDur, float samplerate): Impulse(relativeDur, samplerate), relDur(relativeDur) {
};
