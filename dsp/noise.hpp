#pragma once

#ifndef NOISE_HPP
#define NOISE_HPP

namespace dspheaders {
  class Noise {
    struct M{
      unsigned counter = 0;
      unsigned duration_in_samples = 0;
      float current = 0.0;
      float inc = 0.0;
      float samplerate = 0.0;
      float sr_recip = 0.0;
    } m;

    explicit Noise(M m) : m(std::move(m)) {}

    public:
    static Noise init(float samplerate) {
      float sr_recip = 1.0 / samplerate;
      srand(static_cast<unsigned>(time(0)));
      return Noise(M{
        .counter = 0,
        .duration_in_samples = 0,
        .current = 0.0,
        .inc = 0.0,
        .samplerate = samplerate,
        .sr_recip = sr_recip
      });
    }

    void set_samplerate(float samplerate) {
      m.samplerate = samplerate;
      m.sr_recip = 1.0 / samplerate;
    }

    float play(float duration) {
      m.counter++;
      if (m.counter >= m.duration_in_samples) {
        m.duration_in_samples = static_cast<unsigned>(m.samplerate * duration);
        m.counter = 0;
        float next = random_value(-1.0, 1.0);
        m.inc = (next - m.current) / static_cast<float>(m.duration_in_samples);
      }
      m.current += m.inc;
      return m.current;
    }

    float random_value(float low, float high) {
      return low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
    }
  };
}

#endif
