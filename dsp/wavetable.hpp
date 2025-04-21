#pragma once

#include <array>
#include <cstddef>
#include <stddef.h>
#ifndef WAVETABLE_HPP
#define WAVETABLE_HPP

#include <utility>
#ifndef DEBUG
  #define D(x)  
#else 
  #include <assert.h>
  #define D(x) x
#endif

#pragma once

namespace dspheaders {
  class Wavetable {
    struct M {
      float position = 0.f;
      float samplerate = 0.f;
      float sr_recip = 0.f;
    } m;

    explicit Wavetable(M m) : m(std::move(m)) {}

    public:
    Wavetable(){}
    static Wavetable init(float samplerate) {
      return Wavetable(M{
          .position = 0.f,
          .samplerate = samplerate,
          .sr_recip = 1.f/samplerate
        }
      );
    }

    template <unsigned SIZE, float(*interpolate) (const float, const float* const , const size_t)>
    float play(float* table, float frequency, float phase) {
      D(assert(table != nullptr && "table has not been initialized");)
      if (frequency > m.samplerate * 0.5f) return 0;
      float len = static_cast<float>(SIZE);
      m.position += (len * m.sr_recip * frequency) + (phase * len);
      while (m.position <  0.f) m.position += len;
      while (m.position >= len) m.position -= len;
      return interpolate(m.position, table, SIZE);
    };

    template<unsigned SIZE, float(*interpolate)(const float, const float* const, const size_t)>
    float play(std::array<float, SIZE> table, float frequency, float phase) {
      if (frequency > m.samplerate * 0.5f) return 0;
      float len = static_cast<float>(SIZE);
      m.position += (len * m.sr_recip * frequency) + (phase * len);
      while (m.position <  0.f) m.position += len;
      while (m.position >= len) m.position -= len;
      return interpolate(m.position, table, SIZE);
    }

    void setSamplerate(float samplerate) {
      m.samplerate = samplerate;
      m.sr_recip = 1.0 / samplerate;
    }
  };

  
  class WavetableLinear {
    struct M {
      float position = 0.f;
      float samplerate = 0.f;
      float sr_recip = 0.f;
    } m;

    explicit WavetableLinear(M m) : m(std::move(m)) {}

    public:
    WavetableLinear(){}
    static WavetableLinear init(float samplerate) {
      return WavetableLinear(M{
          .position = 0.f,
          .samplerate = samplerate,
          .sr_recip = 1.f/samplerate
        }
      );
    }

    template <unsigned SIZE>
    float play(float* table, float frequency, float phase) {
      D(assert(table != nullptr && "table has not been initialized");)
      if (frequency > m.samplerate * 0.5f) return 0;
      float len = static_cast<float>(SIZE);
      m.position += (len * m.sr_recip * frequency) + (phase * len);
      while (m.position <  0.f) m.position += len;
      while (m.position >= len) m.position -= len;
      size_t a = m.position;
      size_t b = a + 1;
      float w = m.position - a;
      return table[a] * (1-w) + table[b] * w;
    };

    void setSamplerate(float samplerate) {
      m.samplerate = samplerate;
      m.sr_recip = 1.0 / samplerate;
    }
  };
}

#endif
