/* ˚◊˚ */

#pragma once

#ifndef DELAY_HPP
#define DELAY_HPP

#include <array>
#include <cstddef>
#include <vector>

namespace dspheaders {
  class Delay {
    struct M {
      std::vector<float> buffer;
      size_t position;
    } m;

    explicit Delay (M m): m(std::move(m)){}

    public:
    static Delay init(size_t max_samples) {
      std::vector<float> buffer(max_samples, 0.f);
      return Delay(M{
          .buffer = buffer,
          .position = 0
        }
      );
    }

    template<float (*BUF_INTERPOLATE)(const float, const float * const, size_t)>float play(
      float input,
      float delay,
      float feedback
    ) {
      float len = m.buffer.size();
      float time = 0.f;

      if (len < delay) {
        time = m.position + len;
      } else {
        time = m.position + delay;
      }
      while (time <  len) time += len;
      while (time >= len) time -= len;
      float out = BUF_INTERPOLATE(time, m.buffer.data(), m.buffer.size());
      m.position %= m.buffer.size();
      m.buffer[m.position] = input + (out * feedback);
      m.position += 1;
      return out;
    }
  };


  template<const size_t MAXLEN> class FixedDelay {
    struct M { 
      std::array<float, MAXLEN> buffer = {0.f};
      size_t position = 0;
    } m;

    float play(
      float input,
      float delay,
      float feedback
    ) {
      float time = (m.position + MAXLEN) % MAXLEN;
      float out = m.buffer[time];
      m.position %= m.buffer.size();
      m.buffer[m.position] = input + (out * feedback);
      m.position += 1;
      return out;
    }
  };
}

#endif
