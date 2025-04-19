#pragma once
#include <cstdlib>
#include <tuple>
#ifndef FILTER_HPP
#define FILTER_HPP

#include <cstddef>
#include <vector>

///
///   feedback comb filter (IIR: infinite impulse response)
///
///             ╓───────────────────────────────> out
///             ║   ╓──────────────────────╖ 
///  in ─>( + )─╨─> ║  buffer(n - offset)  ║──╖
///         Λ       ╙──────────────────────╜  ║ 
///         ╙───────────( * feedback ) <──────╜
///

///
///   feedforward comb filter (FIR: finite impulse response)
///
///        ╓────────> ( * amp )─────────────╖
///        ║   ╓──────────────────────╖     V
///  in ───╨─> ║  buffer(n - offset)  ║─> ( + )──> out
///            ╙──────────────────────╜    
///

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

namespace dspheaders {
  // enum COMBTYPE {
  //   FIR,
  //   IIR
  // };

  class Comb {
    protected:
      struct M {
        std::vector<float> buffer;
        size_t bufferlength;
        float damp;
        float previous;
        float feedforward;
        float feedback;
        size_t position;
        size_t delay;
        float previous_in;
        float previous_out;
      } m;

    explicit Comb(M m ): m(std::move(m)){}

    float read();
    void write(float sample);


    public: 
      template<size_t size> static Comb init(float feedforward, float feedback) {
        return Comb(M{
          .buffer = std::vector<float>(size, 0.f),
          .bufferlength = size,
          .feedforward = feedforward,
          .feedback = feedback,
          .position = 0,
          .delay = 0,
        });
      }

      inline unsigned getBufferLength() {
        return m.bufferlength;
      }

      float process(float sample) {
        float delayed = m.buffer[m.position];


        float fb = sample - m.feedback * delayed;
        m.buffer[m.position] = fb;
        float out = m.feedforward * fb + delayed;
        m.position = m.position + 1;
        // handle maximum size of delay line
        while (m.position >= m.bufferlength) m.position -= m.delay;
        return m.feedforward * fb + delayed;
      }
  };


  namespace onepole {
    struct Onepole {
      struct M {
        float prev  = 0.f; 
        float coeff = 0.f;
      } m;
      public: 
      /* m.damp has the range of -1 .. 1
       * m.damp > 0 == lowpass
       * m.damp < 0 == highpass
       * keep |m.damp| < 1 for stability
      */
      auto set_coeff(float coeff) {
        m.coeff = coeff;
      }

      /* m.damp has the range of -1 .. 1
       * m.damp > 0 == lowpass
       * m.damp < 0 == highpass
       * keep |m.damp| < 1 for stability
      */
      inline auto process(float sample) {
        m.prev = ((1.f - std::abs(m.coeff))*sample) + (m.coeff * m.prev);
        return m.prev;;
      }
    };
  }

  namespace state_variable_filter {
    struct SVFCoeffs { 
      float a1 = 0.f;
      float a2 = 0.f;
      float a3 = 0.f;
      float m0 = 0.f;
      float m1 = 0.f;
      float m2 = 0.f;
      float k = 0.f;
      float a = 0.f;

      public:
      inline auto lpf(float w, float q){
        float g = tanf(w/2.0);
        k = 1.0/q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;

        m0 = 0.0;
        m1 = 0.0;
        m2 = 1.0;
      }
      
      inline void bpf(float w, float q) {
        float g = tanf(w/2.0);
        k = 1.0/q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;

        m0 = 0.0;
        m1 = 1.0;
        m2 = 0.0;
      }

      inline void hpf(float w, float q) {
        float g = tanf(w/2.0);
        k = 1.0/q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;

        m0 = 1.0;
        m1 = -k;
        m2 = -1.0;
      }

      inline void notch(float w, float q) {
        float g = tanf(w/2.0);
        k = 1.0/q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;

        m0 = 1.0;
        m1 = -k;
        m2 = 0.0;

      }
      
      inline void peq(float w, float q, float gain) {
        float g = tanf(w/2.0) * sqrt(gain);
        a = powf(10.0, gain / 40.0);
        k = 1.0/q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;

        m0 = 1.0;
        m1 = k * (a * a - 1.0);
        m2 = 0.0;
      }

      inline void low_shelf(float w, float q, float gain) { 
        float g = tanf(w/2.0) * sqrt(gain);
        a = powf(10.0, gain / 40.0);
        k = 1.0/q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 1.0;
        m1 = k * (a - 1.0);
        m2 = a * a - 1.0;
      }

      inline void high_shelf(float w, float q, float gain) { 
        float g = tanf(w/2.0) * sqrt(gain);
        a = powf(10.0, gain / 40.0);
        k = 1.0/q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = a * a;
        m1 = k * (1.0 - a) * a;
        m2 = 1.0 - a * a;
      }
    };

    class SVFilter {
      SVFCoeffs c;
      struct M {
        float ic1eq = 0.f; // integrator capacitor 1 (equalized)
        float ic2eq = 0.f; // integrator capacitor 2 (equalized)
        float curr = 0.f;  // sample of current tick
        float v1, v2, v3;
        float a_sq;
        float filter[6]{0.f};
      } m;

      explicit SVFilter(M m ): m(std::move(m)){}

      enum FilterType {
        LPF = 0,
        BPF,
        HPF,
        LOW_SHELF,
        HIGH_SHELF,
        PEQ
      };

      public:

      static SVFilter init() {{
        return SVFilter(M{
          0.f,
          0.f,
          0.f,
          0.f,
          0.f,
          0.f,
          0.f,
        });
      }}

      SVFilter() {}

      /* This function sets up the values for calling getting any type of
       * filtertype from the filter.
       *
       * Should be followed up with a call to one of the filter type functions:
       * [ lpf, bpf, hpf, low_shelf, high_m, peq ]
       */
      void process(float sample, float w, float q, float gain) {
        float g = tanf(w/2.0) * sqrt(gain);
        c.a = powf(10.0, gain / 40.0);
        c.k = 1.0/q;
        c.a1 = 1.0 / (1.0 + g * (g + c.k));
        c.a2 = g * c.a1;
        c.a3 = g * c.a2;

        // v0 is sample
        //
        // v3 = v0 - m.ic2eq
        // v1 = a1 * m.ic1eq + a2 * v3
        // v2 = m.ic2eq + a2 * m.ic1eq + a3 * v3
        // m.ic1eq = 2 * v1 - m.ic1eq
        // m.ic2eq = 2 * v2 - m.ic2eq
        //
        // output = m0 * v0 + m1 * v1 + m2 * v2

        m.v3 = sample - m.ic2eq;
        m.v1 = c.a1 * m.ic1eq + c.a2 * m.v3;
        m.v2 = m.ic2eq + c.a2 * m.ic1eq + c.a3 * m.v3;
        m.ic1eq = 2.0 * m.v1 - m.ic1eq;
        m.ic2eq = 2.0 * m.v2 - m.ic2eq;
        m.a_sq = c.a * c.a;
      }

      inline float lpf() { return m.v2; }
      inline float bpf() { return m.v1; }
      inline float hpf() { return m.curr + -c.k * m.v1 - m.v2; }
      inline float low_shelf(float gain) { 
        float a = powf(10.0, gain / 40.0);
        return m.curr + (c.k * (a - 1.f) * m.v1) + (a * a -1.0) * m.v2; 
      }
      inline float high_shelf(float gain) { 
        float a = powf(10.0, gain / 40.0);
        float a2 = a*a;
        return a2 * m.curr + (c.k * (1.f - a) * a * m.v1) + (1.f - a2) * m.v2; 
      }
      inline float peq(float gain) { 
        float a = powf(10.0, gain / 40.0);
        float a2 = a*a;
        return m.curr + (c.k * (a2 - 1.f) * m.v1);
      }

      void update(SVFCoeffs coeffs) {
        c = coeffs;
      };

    };
  }
}

#endif
