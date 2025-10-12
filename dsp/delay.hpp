/* ˚◊˚ */

#pragma once

#ifndef DELAY_HPP
#define DELAY_HPP

#include <array>
#include <cstddef>
#include <vector>
#include "interpolation.hpp"

namespace dspheaders {
  template<interpolate_t BUF_INTERPOLATE>
  class Delay {
    private:
    struct M {
      std::vector<float> buffer;
      size_t position;
    } self;
    explicit Delay (M other): self(std::move(other)){}
    public:
    static Delay init(size_t max_samples) {
      std::vector<float> buffer(max_samples, 0.f);
      return Delay(M{ .buffer = buffer, .position = 0 });
    }

    float play( float input, float delay, float feedback) {
      float len = self.buffer.size();
      float time = static_cast<float>(self.position) + delay;
      while (time >= len) time -= len;
      while (time <  0.0) time += len;
      float out = BUF_INTERPOLATE(time, self.buffer.data(), self.buffer.size());
      self.position %= self.buffer.size();
      self.buffer[self.position] = input + (out * feedback);
      self.position += 1;
      return out;
    }
  };


  template<const size_t MAXLEN> 
  class FixedDelay {
    private:
    struct M { 
      std::array<float, MAXLEN> buffer = {0.f};
      size_t position = 0;
    } self;
    explicit FixedDelay (M other): self(std::move(other)){}
    public:
    static FixedDelay init() {
      std::array<float, MAXLEN> buffer;
      std::fill(buffer.begin(), buffer.end(), 0.f);
      return FixedDelay(M{ .buffer = buffer, .position = 0 });
    }

    float play( float input, float delay, float feedback) {
      float time = (self.position + MAXLEN) % MAXLEN;
      float out = self.buffer[time];
      self.position %= self.buffer.size();
      self.buffer[self.position] = input + (out * feedback);
      self.position += 1;
      return out;
    }
  };
}

#endif
