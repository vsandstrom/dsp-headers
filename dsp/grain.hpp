#pragma once
#include "buffer.hpp"
#include "dsp.h"
#include "envelope.hpp"
#include "buffer.hpp"
#include "grain.hpp"

namespace dspheaders {

  class Grain {
    private: 
    // Shared across whole swarm
      Buffer* g_buffer;
      float* g_samplerate;
      
      Envelope* g_envelope;
      
      float m_readptr;
      float m_envptr;
      unsigned m_envlength;
      float m_jitter = 0.f;
      float m_random = 0.f;

      float m_playbackrate = 1.f;
      float m_dur = 0.2;

    public:
      char m_active = 1;

      float play(float delay);
      float play(float delay, float rate);

      // Setter & Getter
      float getoffset(float noise);
      inline void setDur(float dur) {
        // 512 (len) / 48000 (sr) * 0.2 (sec) ≈ 0.05333334 samples/frame
        m_dur = m_envlength / (*g_samplerate * dur) ; };
      inline void setRate(float rate) { m_playbackrate = rate; };
      inline void setJitter(float jitter) {m_jitter = jitter;};

      void test();

    Grain(
      float readptr,
      float graindur,
      float* samplerate,
      Buffer* buffer,
      Envelope* envelope
    );
  };

  class Granulator {
    private: 
    // Shared variables between Granulator and Grain-swarm
      float g_samplerate;
      Buffer g_buffer;

      Grain* g_grains;
      Envelope* g_envelope;

      unsigned m_maxgrains;
      unsigned m_writeptr;
      float m_playbackrate=1.f;

      void write(float sample);

    public:
    // Live variables
      unsigned m_numgrains = 8;
      float m_grainsize = 0.2f;
      float m_jitter = 1.f;

    // Setters

      inline void setNumGranulators(int num)    { m_numgrains = num;  };
      inline void setJitter (float amount) { 
      };
      inline void setGrainSize(float dur) { m_grainsize = dur; };
      inline void setRate(float rate) { m_playbackrate = rate; };

    // Process / Play

      float process(float input, float offset);
      float process(float input, float offset, int trigger);

      float process(float input, float offset, float rate);
      float process(float input, float offset, float rate, int trigger);

    // Construct / Destroy

      // Default envelope - works fine to get going
      Granulator(
        float samplerate,
        unsigned maxgrains,
        float (*interpolate)(float, float*, unsigned)
      );
     
      // Predefined grain envelope in float array
      Granulator(
        float samplerate, 
        float* envtable, 
        unsigned tablelength,
        unsigned maxgrains,
        float (interpolate)(float, float*, unsigned)
      );
      
      // Predefined grain envelope in Wavetable
      Granulator(
        float samplerate,
        Envelope* grainEnvelope
      );

      ~Granulator();
  };
}
