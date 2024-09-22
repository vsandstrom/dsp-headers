#include "waveshape.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <array>
#ifndef DEBUG
  #define D(x)  
#else 
  #include <assert.h>
  #define D(x) x
#endif

static size_t count = 0;

namespace dspheaders {
  template<size_t NUMGRAINS, size_t BUFSIZE>
  class GranulatorEX {
    struct Grain {
      public:
      float bufpos = 0.f;
      float envpos = 0.f;
      float rate = 1.f;
      float dur = 0.f;
      bool  active = false;
    };

    struct M{
      float* buffer = new float[BUFSIZE];
      float buflen = static_cast<float>(BUFSIZE);

      float* env;
      size_t envlen;

      size_t recpos = 0;
      bool recording = true;

      float samplerate = 0.f;
      float sr_recip = 0.f;
      
      size_t next;
      std::array<Grain, NUMGRAINS> grains;
    } m;

    explicit GranulatorEX<NUMGRAINS, BUFSIZE> (M m): m(std::move(m)){}

    public:
    static GranulatorEX init(
      float samplerate
    ) {

      size_t envlen = 512;
      float* env = new float[envlen];
      hanning(env, envlen);

      return GranulatorEX(M{
        .env = env,
        .envlen = envlen,
        .samplerate = samplerate,
        .sr_recip = 1.f / samplerate,
      });
    }

    template<float (*BUF_INTERPOLATE)(float, float*, size_t), float (*ENV_INTERPOLATE)(float, float*, size_t)>
    float play(
        float position,
        float duration,
        float rate,
        float jitter,
        float trigger
      ) {
      // D(assert(m.grains[m.next] != nullptr && "grains has not been initialized"));
      if (trigger >= 1.f && !m.grains[m.next].active) {
        // normalize buffer position
        // float pos = (position + jitter) - long(position+jitter);
        // while (pos < 0.f) pos += 1.f;
        while (position > 1.0f) position -= 1.f;
        while (position < 0.0f) position += 1.f;

        D(printf("trig %zu", count++));
        
        // set params for grain
        m.grains[m.next].bufpos = position * m.buflen;
        m.grains[m.next].envpos = 0.f;
        m.grains[m.next].rate = rate;
        m.grains[m.next].dur = calc_duration(
          static_cast<float>(m.envlen),
          m.sr_recip,
          1.f / duration
        );
        m.grains[m.next].active = true;
        m.next = (m.next + 1) % NUMGRAINS;
      }

      // loop over active grains and deactivate stale grains
      float out = 0.f;
      size_t len = static_cast<size_t>(m.buflen);

      for (Grain &g: m.grains) {
        if (g.envpos >= m.envlen) {g.active = false; continue;}
        if (g.active) {
          float sig = BUF_INTERPOLATE(g.bufpos, m.buffer, len);
          float env = ENV_INTERPOLATE(g.envpos, m.env, m.envlen);
          g.bufpos += g.rate;
          while (g.bufpos >= len) { g.bufpos -= len; } 
          g.envpos += g.dur;
          while (g.envpos >= m.envlen) { g.envpos -= m.envlen; } 
          out += sig * env;
        }
      }
      return out;
    }

    float calc_duration(float envlen, float sr_recip, float dur_recip) {
      return envlen * sr_recip * dur_recip;
    }

    ~GranulatorEX(){
      delete[] m.buffer;
    }

    void reset_record() { 
      m.recpos = 0; 
      m.recording = true;
    }

    void update_envelope(float*env, size_t envlen) {
      delete[] m.env;
      m.env = new float[envlen];
      for (int i = 0; i < envlen; i++) { m.env[i] = env[i]; }
      m.envlen = envlen;
    }

    void set_samplerate(float samplerate) { 
      m.samplerate = samplerate; 
      m.sr_recip = 1.f / samplerate; 
    }

    void set_buffersize(size_t size) { 
      delete[] m.buffer;
      m.buffer = new float[size];
      m.buflen = size;
    }

    bool record(float sample) {
      if (m.recpos == m.buflen) { m.recording = false; return m.recording; } 
      m.buffer[m.recpos] = sample;
      m.recpos++;
      return m.recording;
    }
  };
}
