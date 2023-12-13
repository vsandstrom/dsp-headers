#pragma once

#include "buffer.hpp"
#include "dsp.h"
#include "envelope.hpp"
#include "interpolation.hpp"
#include "wavetable.hpp"
#include "buffer.hpp"

namespace dspheaders {

  class Grain {
    private: 
    // Shared across whole swarm
      Buffer* buffer;
      Envelope* envelope;

    // Set on spawn
      float readptr;
      float envptr;
      float playbackspeed = 1.f;
      float graindur = 0.2;
      unsigned envlength;
      float samplerate;

    public:
      char active = 1;

      float play(float readposition);
      float play(float readposition, float speed);
      void setDur(float dur);
      float getoffset(float noise);
      void setSpeed(float speed);

    Grain(
      float readptr,
      float graindur,
      float samplerate,
      Buffer* buffer,
      Envelope* envelope
    );
  };

  class Granulator {
    private: 
    // Static variables
      float samplerate;
      Envelope grainenv;
      unsigned maxgrains;
      Buffer buffer;
      unsigned writeptr;
      Grain* grains;

    public:
    // Live variables
      unsigned numgrains = 8;
      float grainsize = 0.2f;
      float jitter = 0.f;

    // Setters

      inline void setNumGranulators(int num)    { numgrains = num;  };
      inline void setJitter (float amount) { 
      };
      inline void setGrainSize(float dur) { grainsize = dur; };

    // Process / Play

      float process(float offset);
      float process(float offset, GATE trigger);
      float process(float offset, GATE trigger, float jitter);
      float process(float offset, GATE trigger, float jitter, float grainsize);

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
        float* table, 
        unsigned tablelength,
        unsigned maxgrains,
        float (interpolate)(float, float*, unsigned)
      );
      
      // Predefined grain envelope in Wavetable
      Granulator(
        float samplerate,
        Envelope grainEnvelope
      );

      ~Granulator();
  };
}
