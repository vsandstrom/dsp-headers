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
      float previn = 0.f;
      float prevout = 0.f;
      float read(unsigned readptr);
      void write(float sample);
      unsigned writeptr = 0;
      unsigned readptr = 0;
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
      float play(float sample, float ff, float fb);
      Comb(
          unsigned offset,
          unsigned samplerate
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
      float play(float sample, float coeff);
      float play2(float sample, float coeff);
      float play(float sample, float coeff, float mod);
      Allpass(
        unsigned offset,
        unsigned samplerate
      );
  };
}


