#pragma once
#include "buffer.hpp"

namespace dspheaders {
  class Delay {
    protected:
      // magic number... feedback goes too hard
      float fb_scale_coeff = 0.5;
      Buffer buffer;
      unsigned samplerate;
      unsigned delay_taps = 1;
      float time;

      float prev = 0.f;
      unsigned writeptr = 0;
      // Read sample from delay buffer with 
      // delaytime * samplerate number of samples offset
      virtual float read(float delaytime, float damp);
      // Write sample to delay buffer
      virtual void write(float sample);

    public:
      // Set the number of taps in the delay.
      void taps(unsigned taps);

      // Set the duration between delay taps
      void delaytime(float delaytime);
      void write(float sample, int offset);
      float read(int offset);

      virtual float process(float input, float delaytime, float wet, float feedback);
  
      // Initialize Delay
      // maxdelaytime: the size of the buffer in samples
      Delay(
        unsigned samplerate,
        float maxdelaytime,
        unsigned delay_taps,
        float (*interpolate)(float, float*, unsigned)
      );
     
      // Initialize Delay without assigning a number of delay taps
      Delay(
        unsigned samplerate,
        float maxdelaytime,
        float (*interpolate)(float, float*, unsigned)
      );
  };

  class IDelay : public Delay {
    void write();

    float process(float input, float delaytime, float wet, float feedback);
    float read(float delaytime, float damp);
    void write(float sample);

  };
}
