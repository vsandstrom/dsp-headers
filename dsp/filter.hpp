#pragma once
#include "buffer.hpp"
#include "dsp_math.h"
#include "dsp.h"

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
///              ╓───> ( * coeff )──────────────────╖
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

///
///                 biquad filter
///
///            
///             
///  in ─> ( + )───────( * a0 )────╖──────( + )──> out
///          Λ                     ║             
///          ╙──( - )──( * a1 )<─|n-1|->( * b1 )
///          Λ                     ║         
///          ╙──( - )──( * a2 )<─|n-2|->( * b2 )
///
///

  class Biquad {
    // 2-sample delayline on input side
    float x1 = 0.f, x2 = 0.f;
    // 2-sample delayline on output side
    float y1 = 0.f, y2 = 0.f;
    // feedforward coeffs
    float b0 = 0.f, b1 = 0.f, b2 = 0.f;
    // feedback coeffs
    float a0 = 1.f, a1 = 0.f, a2 = 0.f;
    // dc block
    float x = 0.f, y = 0.f;
      
    constexpr void normalizeCoeffs() {
      b0 /= a0;
      b1 /= a0;
      b2 /= a0;
      a1 /= a0;
      a2 /= a0;
    }

    public:
      Biquad(){}

      inline float process(float input) {
        float output = b0*input + b1*x1 + b2*x2 - a1*y1 - a2*y2;
        x2 = x1;
        x1 = input;
        y2 = y1;
        y1 = output;
        return output;
      }

      
      // Low Pass Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      constexpr void calcLPF(float omega, float q) {
        float alpha = sin(omega) / (2 * q);

        // See order:
        b1 = 1 - cos(omega);      //  1-cos(omega)
        b0 = b1 / 2;              // (1-cos(omega)) / 2
        b2 = b0;                  // (1-cos(omega)) / 2

        a0 = 1 + alpha;           //  1 + alpha
        a1 = -2 * cos(omega);     // -2 * cos(omega)
        a2 = 1 - alpha;           //  1 - alpha
        normalizeCoeffs();
      }

      // Band Pass Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      constexpr void calcBPF(float omega, float q) {
        float alpha = sin(omega) / (2 * q);

        b0 = alpha;               // alpha
        b1 = 0;                   // 0
        b2 = -alpha;              // -alpha

        a0 = 1 + alpha;           //  1 + alpha
        a1 = -2 * cos(omega);     // -2 * cos(omega)
        a2 = 1 - alpha;           //  1 - alpha
        normalizeCoeffs();
      }

      // Band Pass Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      // float gain = Gain in dB
      constexpr void calcPEQ(float omega, float q, float gain) {
        float alpha = sin(omega) / (2 * q);
        float A = powf(10, gain/40.f);

        b0 = 1 + alpha * A;       // 1 + alpha * A
        b1 = -2 * cos(omega);     // -2 * cos(omega)
        b2 = 1 - alpha * A;       // 1 - alpha * A

        a0 = 1 + alpha / A;       //  1 + alpha
        a1 = -2 * cos(omega);     // -2 * cos(omega)
        a2 = 1 - alpha / A;       //  1 - alpha / A
        normalizeCoeffs();
      }

      // Notch Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      constexpr void calcNotch(float omega, float q) {
        float alpha = sin(omega) / (2 * q);

        b0 = 1;                   //  1
        b1 = -2 * cos(omega);     // -2 * cos(omega)
        b2 = 1;                   //  1

        a0 = 1 + alpha;           //  1 + alpha
        a1 = b1;                  // -2 * cos(omega)
        a2 = 1 - alpha;           //  1 - alpha
        normalizeCoeffs();
      }

      // High Pass Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      constexpr void calcHPF(float omega, float q) {
        float alpha = sin(omega) / (2 * q);

        b0 = (1 + cos(omega)) / 2;  //  (1 + cos(omega)) / 2
        b1 = -(b0 * 2);             // -(1 + cos(omega))
        b2 = b0;                    //  (1 + cos(omega)) / 2

        a0 = 1 + alpha;             //  1 + alpha
        a1 = -2 * (cos(omega));     // -2 * cos(omega)
        a2 = 1 - alpha;             //  1 - alpha
        normalizeCoeffs();
      }
  };
}
