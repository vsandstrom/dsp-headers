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
}
