#include "dsp.h"
#include "envelope.hpp"
#include <cstddef>
#include <limits>
#include <vector>

namespace dspheaders {
  template<size_t NUMGRAINS, size_t BUFSIZE>
  class Granulator2 {
    private:

    struct Grain {
      float bufpos    = 0.f;
      float envpos    = 0.f;
      float durations = 0.f;
      float rate     = 0.f;
      bool  active    = false;
    };

    std::vector<float> m_buf;
    float m_bufsize = 0.0f;

    Envelope m_env;
    float m_envsize = 0.0f;
    size_t m_recpos = 0;
    size_t m_next = 0;

    Grain m_grains[NUMGRAINS]{Grain()};

    float m_samplerate;
    float m_sr_recip;
    float (*interpolate)(float, float*, unsigned);

    inline constexpr float calc_position(float pos) {
      float x = pos - (long)pos;
      return (x) < 0.0 ? 1.0f + x : x;
    }

    inline constexpr float calc_duration(size_t env_len, float sr_recip, float duration) {
      return env_len * sr_recip * duration;
    }

    public:
    template<size_t VALUES, size_t DURATIONS>
    Granulator2(
        BreakPoints brk,
        float samplerate,
        float (*interpolate)(float, float*, unsigned)
      ):
      m_buf(BUFSIZE, 0.f), 
      m_bufsize(BUFSIZE),
      m_env(brk, samplerate, interpolate),
      m_envsize(sum(brk.durations, DURATIONS)),
      m_samplerate(samplerate),
      m_sr_recip(1.0f / samplerate)

    { }


    Granulator2(
        float* table,
        unsigned tablelength,
        float samplerate,
        float (*interpolate)(float, float*, unsigned)
      ):
      m_buf(BUFSIZE, 0.f), 
      m_bufsize(BUFSIZE),
      m_env(table, tablelength, samplerate, interpolate), 
      m_envsize(tablelength),
      m_samplerate(samplerate),
      m_sr_recip(1.0f / samplerate)
    { }


    inline float play(float pos, float dur, float rate, float jitter, float trig) {
      if (abs(trig - 1.0f) < std::numeric_limits<float>::epsilon() ) {
        const float position = calc_position(pos + jitter);
        const float duration = calc_duration(m_envsize, m_sr_recip, dur);
        m_grains[m_next].bufpos    = position;
        m_grains[m_next].envpos    = 0.0;
        m_grains[m_next].rate      = rate;
        m_grains[m_next].durations = duration;
        m_grains[m_next].active    = true;
        m_next = (m_next + 1) % NUMGRAINS;
      }

      float out = 0.f;
      for (auto &grain: m_grains) {
        if (grain.envpos >= m_envsize) { grain.active = false; }
        if (grain.active) {
          float sig = interpolate(grain.bufpos, m_buf.data(), m_buf.size());
          float env = m_env.read(grain.envpos);

          grain.bufpos += grain.rate;
          grain.envpos += grain.durations;
          out += (sig * env);
        }
      }
      return out;
    }

    inline bool record(float sample) {
      // fills the whole buffer before before returning false
      if (m_recpos == BUFSIZE) { return false; }
      m_buf[m_recpos] = sample;
      return true;
    }

    inline void reset() {
      m_recpos = 0;
    }
  };
} // namespace::dspheaders
