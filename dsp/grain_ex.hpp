#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <vector>

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

      float* env = new float [512];
      size_t envlen = 512;

      size_t recpos = 0;
      bool recording = true;

      float samplerate = 0.f;
      float sr_recip = 0.f;
      
      size_t next;
      std::vector<Grain> grains = std::vector<Grain>(NUMGRAINS, GranulatorEX::Grain{
          .bufpos = 0.f,
          .envpos = 0.f,
          .rate = 1.f,
          .dur = 0.f,
          .active = false
        }
      );
    } m;

    explicit GranulatorEX<NUMGRAINS, BUFSIZE> (M m): m(std::move(m)){}

    public:
    static GranulatorEX init(
      float samplerate
    ) {
      return GranulatorEX(M{
        .samplerate = samplerate,
        .sr_recip = 1.f / samplerate,
      });
    }

    template<float (*BUF_INTERPOLATE)(float, float*, unsigned), float (*ENV_INTERPOLATE)(float, float*, unsigned)>
    float play(
        float position,
        float duration,
        float rate,
        float jitter,
        float trigger
      ) {
      if (trigger >= 1.f && !m.grains[m.next].active) {
        // normalize buffer position
        // float pos = (position + jitter) - long(position+jitter);
        // while (pos < 0.f) pos += 1.f;
        while (position > 1.0f) {
          position -= 1.f;
        }
        while (position < 0.f) {
          position += 1.f;
        }
        
        // set params for grain
        size_t n = m.next;
        m.grains[n].bufpos = position * m.buflen;
        m.grains[n].envpos = 0.f;
        m.grains[n].rate = rate;
        m.grains[n].dur = calc_duration(
          static_cast<float>(m.envlen),
          m.sr_recip,
          1.f / duration
        );
        m.grains[n].active = true;
        m.next = (n + 1) % NUMGRAINS;
      }

      // loop over active grains and deactivate stale grains
      float out = 0.f;
      size_t len = static_cast<size_t>(m.buflen);

      for (auto &g: m.grains) {
        if (g.envpos >= m.envlen) {g.active = false; continue;}
        if (g.active) {
          float sig = BUF_INTERPOLATE(g.bufpos, m.buffer, len);
          float env = ENV_INTERPOLATE(g.envpos, m.env, m.envlen);
          g.bufpos += g.rate;
          while (g.bufpos >= len) { g.bufpos -= len; } 
          g.envpos += g.dur;
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
