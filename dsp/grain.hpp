#pragma once

#include "buffer.hpp"
#include "envelope.hpp"
#include "interpolation.hpp"
#include "wavetable.hpp"
#include "buffer.hpp"

using namespace dspheaders;

class Grain {
  private: 
  // Shared across whole swarm
    Buffer* buffer;
    Wavetable* envelope;

  // Set on spawn
    float readptr;
    float envptr;
    // Increment speed
    float speed;
    float graindur = 0.2;
    unsigned envlength;
    float samplerate;

  public:
    float getoffset(float noise);

  // 
    float play(float offset);

    void setDur(float dur);

  Grain(
    float readptr,
    float speed,
    float graindur,
    float samplerate,
    Buffer* buffer,
    Wavetable* envelope
  );
};

class Granulator {
  private: 
  // Static variables
    float samplerate;
    Wavetable grainenv;
    unsigned maxgrains;
    Buffer buffer;
    unsigned writeptr;
    Grain* grains;

  public:
  // Live variables
    unsigned numgrains = 8;
    float jitter = 0.0f;
    float grainsize = 0.2f;

  // Setters

    inline void setNumGranulators(int num)    { numgrains = num;  };
    inline void setJitter (float amount) { jitter = amount;  };
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
      Wavetable grainEnvelope
    );

    ~Granulator();
};
