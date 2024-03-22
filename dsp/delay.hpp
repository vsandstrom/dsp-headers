#pragma once
#include "buffer.hpp"

// Note: Delay uses Buffer class, which size is always a power of two, to
// simplify wrapping of the write head.

namespace dspheaders {
  class Delay {
    protected:
      Buffer m_buffer;
      unsigned g_samplerate;
      // magic number... feedback goes too hard
      unsigned m_taps = 1;
      float m_time;

      unsigned m_writeptr = 0;
      unsigned m_pos_mask;
      float m_prev = 0.f;

      // Read sample from delay buffer with 
      // delaytime * samplerate number of samples offset
      virtual float read(float delaytime, float damp);

      // Write sample to delay buffer
      virtual void write(float sample);

    public:
      // Set the number of taps in the delay.
      inline void taps(unsigned taps) { m_taps = taps; }

      // Set the duration between delay taps
      inline void delaytime(float time) { m_time = time; }


      // Read / write for reverb
      virtual void write(float sample, int offset);
      virtual float read(int offset);

      virtual float process(float input, float feedback);
  
      // Initialize Delay
      // maxdelaytime: the size of the buffer in samples
      Delay(
        unsigned samplerate,
        float time,
        float maxtime,
        unsigned m_taps,
        float (*interpolate)(float, float*, unsigned)
      );
     
      // Initialize Delay without assigning a number of delay taps
      Delay(
        unsigned samplerate,
        float time,
        float maxtime,
        float (*interpolate)(float, float*, unsigned)
      );
  };

  class IDelay : public Delay {
    void write();

    float process(float input, float time, float wet, float feedback);
    float read(float delaytime, float damp);
    void write(float sample);

  };
}
