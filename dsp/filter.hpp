#pragma once
#include "buffer.hpp"

namespace dspheaders {
  enum COMBTYPE {
    FIR,
    IIR
  };

  class Comb {
    protected:
      Buffer m_buffer;
      float m_prev = 0.f;

      float m_read();
      void m_write(float sample);
      unsigned m_writeptr = 0;
      float m_readptr = 0.f;
      float m_damp = 0.f;

///
///   feedback comb filter (IIR: infinite impulse response)
///
///             ╓───────────────────────────────> out
///             ║   ╓──────────────────────╖ 
///  in ─>( + )─╨─> ║  buffer(n - offset)  ║──╖
///         Λ       ╙──────────────────────╜  ║ 
///         ╙───────────( * feedback ) <──────╜
///
      float m_feedback(float sample, float feedback);
      // float m_feedback(float sample, float feedback, float mod);
///
///   feedforward comb filter (FIR: finite impulse response)
///
///        ╓────────> ( * amp )─────────────╖
///        ║   ╓──────────────────────╖     V
///  in ───╨─> ║  buffer(n - offset)  ║─> ( + )──> out
///            ╙──────────────────────╜    
///
      float m_feedforward(float sample, float amp);
      // float m_feedforward(float sample, float amp, float mod);


    public: 
      // Feedback
      float process(float sample, float feedback, COMBTYPE type);
      float process(float sample, float feedback, float mod, COMBTYPE type);
      inline void setDamp(float damp) {
        m_damp = damp;
      }

      inline unsigned getBufferLength() {
        return m_buffer.bufferlength;
      }



      Comb(
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
      );
  };

  class Allpass : public Comb {
///
///                 allpass filter
///
///              ╓───> ( * coeff )────────────────────╖
///              ║   ╓──────────────────────╖       V
///  in ─> ( + )─╨─> ║  buffer(n - offset)  ║─╥─> ( + )──> out
///          Λ       ╙──────────────────────╜ ║ 
///          ╙───────( * (-coeff)) <───╜
///
///       where: amp == feedback 
    public:
      float process(float sample, float coeff);
      float process(float sample, float coeff, float mod);

      Allpass(
        unsigned offset,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
  };


  class Biquad {
    float buffer[2] {0.f};

    // feedforward coeffs
    const float b[3] = {1.f, 0.f, -1.f};
    // feedback coeffs
    float a[3] = { 1.f, -.3835f, .8786f };

    public:
    Biquad(){}
    // Supply custom feedback coeffs, must be *at least* 3, only 3 values will 
    // be read if longer
    Biquad(const float* coeffs) {
      for (unsigned i = 0; i<3; ++i) {
        a[i] = coeffs[i];
      }
    }

    float process(float input, float initial_amplitude) {
      float in = input * initial_amplitude;
      float out = in * b[0] + buffer[0] * b[1] + buffer[1] * b[2];
      float next = buffer[0] * a[0] + buffer[1] * a[1];

      buffer[1] = buffer[0];
      buffer[0] = in + next;
      return out;
    }
  };
}
