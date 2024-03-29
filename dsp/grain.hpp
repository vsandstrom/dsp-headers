#pragma once
#include "buffer.hpp"
#include "envelope.hpp"
#include "buffer.hpp"
#include <memory>


namespace dspheaders {

  class Grain {
    private: 
    // Shared across whole swarm
      std::shared_ptr<Buffer> g_buffer;
      std::shared_ptr<float> g_samplerate;
      std::shared_ptr<Envelope> g_envelope;

      
      float m_readptr;
      float m_envptr;
      unsigned m_envlength;
      float m_jitter = 0.f;
      float m_random = 0.f;

      float m_playbackrate = 1.f;
      float m_dur = 0.0533333;

    public:
      bool m_active = false;

      float play();
      float play(float position, float rate);

      inline void setDur(float dur) {
        // 512 (len) / 48000 (sr) * 0.2 (sec) ≈ 0.05333334 samples/frame
        m_dur = (float)m_envlength / ((*g_samplerate) * dur); };

      inline void setRate(float rate) { m_playbackrate = rate; };
      inline void setJitter(float jitter) {m_jitter = jitter;};

      void test();

    Grain(
      float readptr,
      float graindur,
      float* samplerate,
      std::shared_ptr<Buffer> buffer,
      std::shared_ptr<Envelope> envelope
    );

    Grain();

  };

  class Granulator {
    private: 
    // Shared variables between Granulator and Grain-swarm
      float g_samplerate;
      std::shared_ptr<Buffer> g_buffer;
      std::shared_ptr<Envelope> g_envelope;

      Grain* m_grains;
      const unsigned m_maxgrains = 32;

      float m_position = 0.f;
      float m_playbackrate=1.f;
    public:
    // Live variables
      unsigned m_numgrains = 8;
      float m_grainsize = 0.2f;
      float m_jitter = 1.f;

    // Setters

      inline void setNumGrains(int num)    { m_numgrains = num;  };
      inline void setJitter (float amount) { 
      };
      inline void setGrainSize(float dur) {
        m_grainsize = dur;
        for (int i = 0; i < m_maxgrains; i++) {
          m_grains[i].setDur(dur);

        }
      };
      inline void setRate(float rate) { m_playbackrate = rate; };

    // Process / Play

      float process();
      float process(float position, float trigger);

      // float process(float position, float rate);
      float process(float position, float rate, float trigger);

    // Construct / Destruct

      // Default envelope - works fine to get going
      Granulator(
        float dur,
        float samplerate,
        unsigned maxgrains,
        std::shared_ptr<Buffer> buffer,

        float (*interpolate)(float, float*, unsigned)
      );
     
      // Predefined grain envelope in float array
      Granulator(
        float dur,
        float samplerate, 
        unsigned maxgrains,
        float* envtable, 
        unsigned tablelength,
        float (interpolate)(float, float*, unsigned),
        std::shared_ptr<Buffer> buffer

      );
      
      // Predefined grain envelope in Wavetable
      Granulator(
        float dur,
        float samplerate,
        unsigned maxgrains,
        std::shared_ptr<Envelope> grainEnvelope,
        std::shared_ptr<Buffer> buffer
      );



      ~Granulator();
  };
}
