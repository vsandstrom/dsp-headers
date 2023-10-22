#pragma once
#include "dsp.h"
#include "buffer.hpp"

namespace dspheaders {
  class Delay {
    protected:
      Buffer buffer;
      unsigned samplerate;
      unsigned delay_taps = 1;
      float time;
      unsigned writeptr = 0;

      // Read sample from delay buffer with 
      // delaytime * samplerate number of samples offset
      float read(float delaytime);
      // Write sample to delay buffer
      void write(float sample);
      

    public:
      // Set the number of taps in the delay.
      void taps(unsigned taps);

      // Set the duration between delay taps
      void delaytime(float delaytime);
      void write(float sample, int offset);
      float read(int offset);

      float play(float input, float delaytime, float wet, float feedback);

      // Initialize Delay
      Delay(
        unsigned samplerate,
        float time,
        unsigned delay_taps
      );
     
      // Initialize Delay without assigning a number of delay taps
      Delay(
        unsigned samplerate,
        float time
      );

  };
}
