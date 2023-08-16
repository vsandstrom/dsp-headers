#pragma once
#include "dsp.h"
#include "buffer.hpp"

namespace dspheaders {
  class Delay {
    protected:
      Buffer buffer;
      unsigned samplerate;
      unsigned delay_taps = 1;
      float time = 0.2;
      int writeptr = 0;

      // Read sample from delay buffer with 
      // delaytime * samplerate number of samples offset
      virtual float read(float delaytime);
      // Write sample to delay buffer
      virtual  void write(float sample);

    public:
      // Set the number of taps in the delay.
      void taps(unsigned taps);

      // Set the duration between delay taps
      void delaytime(float delaytime);


      float play(float input, float delaytime, float wet, float feedback);

      // Initialize Delay
      Delay(
        unsigned samplerate,
        float time,
        unsigned delay_taps,
        float (*interpolate)(float, float*, unsigned)
      );
     
      // Initialize Delay without assigning a number of delay taps
      Delay(
        unsigned samplerate,
        float time,
        float (*interpolate)(float, float*, unsigned)
      );

  };
}
