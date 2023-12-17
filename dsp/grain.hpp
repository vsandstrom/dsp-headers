#pragma once
#include "buffer.hpp"
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

      float m_playbackrate = 1.f;
      float m_dur = 0.2;

    public:
      char m_active = 1;

      float play(float delay);
      float play(float delay, float rate);

      // Setter & Getter
      void setDur(float dur);
      float getoffset(float noise);
      void setRate(float rate);
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
      float process(float input, float offset, float rate);

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
