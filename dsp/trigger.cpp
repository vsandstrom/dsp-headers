#include "trigger.hpp"
#include <stdlib.h>
using namespace dspheaders;


float Impulse::play() {
  float out = 0.f;
  if (m_count >= (m_dur * samplerate)) {
    m_count = 0;
    out = 1.f;
  } else {
    m_count++;
  }
  return out;
}

float Impulse::play(float dur) {
  m_dur = dur;
  float out = 0.f;
  if (m_count >= (m_dur * samplerate)) {
    m_count = 0;
    out = 1.f;
  } else {
    m_count++;
  }
  return out;

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
    return 1.f;
  }
  m_count++;
  return 0.f;
}

Dust::Dust(float relativeDur, float samplerate): Impulse(relativeDur, samplerate), relDur(relativeDur) {
};
