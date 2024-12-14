#pragma once

#ifndef ENV_HPP
#define ENV_HPP

#include "dsp.h"
#include "buffer.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>
#include <cassert>

namespace dspheaders {
  struct BreakPoint {
    float value;
    float duration;
    float curve;
  };

  enum class Reset {
    /// creates discontinuities, snaps to first value in envelope.
    HARD,
    /// handles retriggering without discontinuities, uses previous value
    /// and next segment to calculate a new trajectory.
    SOFT
  };

  class Envelope {
    struct M {
      std::vector<BreakPoint> breakpoints;
      float counter;
      size_t segment;
      size_t steps;
      float inc;
      float previous_value;
      float rate;
      float samplerate;
      bool playing;
      bool looping;
      Reset reset;
    } m;

    explicit Envelope(M m): m(std::move(m)){};

    public: 
      static Envelope init(std::vector<BreakPoint> breakpoints, float samplerate) {
        assert(breakpoints.size() > 2 && "BreakPoints need to be at least 2 items long");
        return Envelope(M{
            .breakpoints = breakpoints,
            .counter = 0.f,
            .segment = 0,
            .steps = 0,
            .inc = 0.f, 
            .previous_value = 0.f,
            .rate = 1.f,
            .samplerate = samplerate,
            .playing = false,
            .looping = false,
            .reset = Reset::HARD
          }
        );
      }

      inline void trig() {
        switch (m.reset) {
          case Reset::HARD: {
            m.previous_value = m.breakpoints.front().value;
            break;
          }
          case Reset::SOFT: {
            if (m.previous_value == 0.f) {
              m.previous_value = m.breakpoints.front().value;
            }
            break;
          }
        }
        m.segment = 0;
        m.playing = true;
      }

      inline float play() {
        if (m.playing) {
          if (m.segment < m.breakpoints.size()) {
            BreakPoint brk = m.breakpoints.at(m.segment);
            if (m.segment == 0 || m.counter >= static_cast<float>(m.steps)) {
              m.steps = brk.duration * m.samplerate;
              float angle = brk.value - m.previous_value;
              m.inc = angle / static_cast<float>(m.steps);
              m.segment++;
              m.counter = 0.f;
              return m.previous_value;
            } else {
              m.previous_value += m.inc;
              m.counter += m.rate;
              return m.previous_value;
            }
          } else if (m.segment == m.breakpoints.size()) {
            m.previous_value += m.inc;
            m.counter += m.rate;
            return m.previous_value;
          } else {
            if (m.counter >= static_cast<float>(m.steps)) {
              if (m.looping) {
                m.previous_value = m.breakpoints.front().value;
                m.segment = 0;
              } else {
                m.playing = false;
              }
            }
          }
        }
        return 0.0;
      }

      inline void set_reset_type(Reset reset) {
        m.reset = reset;
      }

      inline void set_loopable(bool loopable) {
        m.looping = loopable;
      }
  };
} // namespace dspheaders

#endif
