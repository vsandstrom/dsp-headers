#pragma once
#include "buffer.hpp"

namespace dspheaders {
  enum COMBTYPE {
    FIR,
    IIR
  };

  class Comb {
    protected:
      Buffer buffer;
      float previn = 0.00001f;
      float prevout = 0.00001f;
      float read(float readptr);
      void write(float sample);
      unsigned writeptr = 0;
      float readptr = 0.f;
///
///   feedback comb filter (IIR: infinite impulse response)
///
///             ╓───────────────────────────────> out
///             ║   ╓──────────────────────╖ 
///  in ─>( + )─╨─> ║  buffer(n - offset)  ║──╖
///         Λ       ╙──────────────────────╜  ║ 
///         ╙───────────( * feedback ) <──────╜
///
      float iir(float sample, float feedback);
      float iir(float sample, float feedback, float mod);
///
///   feedforward comb filter (FIR: finite impulse response)
///
///        ╓────────> ( * amp )─────────────╖
///        ║   ╓──────────────────────╖     V
///  in ───╨─> ║  buffer(n - offset)  ║─> ( + )──> out
///            ╙──────────────────────╜    
///
      float fir(float sample, float amp);
      float fir(float sample, float amp, float mod);


    public: 
      // Feedback
      float play(float sample, float feedback, COMBTYPE type);
      float play(float sample, float feedback, float mod, COMBTYPE type);
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
///              ╓───> ( * amp )────────────────────╖
///              ║   ╓──────────────────────╖       V
///  in ─> ( + )─╨─> ║  buffer(n - offset)  ║─╥─> ( + )──> out
///          Λ       ╙──────────────────────╜ ║ 
///          ╙───────( * (-feedback)) <───╜
///
///       where: amp == feedback 
    public:
      float play(float sample, float coeff);
      float play(float sample, float coeff, float mod);
      Allpass(
        unsigned offset,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
  };
}


