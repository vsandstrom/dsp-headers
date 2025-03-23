#pragma once
#include <cstdlib>
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
      } self;

    explicit Comb(M self ): self(std::move(self)){}

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
        return self.bufferlength;
      }

      float process(float sample) {
        float delayed = self.buffer[self.position];


        float fb = sample - self.feedback * delayed;
        self.buffer[self.position] = fb;
        float out = self.feedforward * fb + delayed;
        self.position = self.position + 1;
        // handle maximum size of delay line
        while (self.position >= self.bufferlength) self.position -= self.delay;
        return self.feedforward * fb + delayed;
      }
  };


  namespace onepole {
    struct Onepole {
      struct M {
        float prev  = 0.f; 
        float coeff = 0.f;
      } self;
      public: 
      /* self.damp has the range of -1 .. 1
       * self.damp > 0 == lowpass
       * self.damp < 0 == highpass
       * keep |self.damp| < 1 for stability
      */
      auto set_coeff(float coeff) {
        self.coeff = coeff;
      }

      /* self.damp has the range of -1 .. 1
       * self.damp > 0 == lowpass
       * self.damp < 0 == highpass
       * keep |self.damp| < 1 for stability
      */
      inline auto process(float sample) {
        self.prev = ((1.f - std::abs(self.coeff))*sample) + (self.coeff * self.prev);
        return self.prev;;
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

      public:
      inline auto lpf(float w, float q) -> SVFCoeffs {
        float g = tanf(w/2.0);
        float k = 1.0/q;
        float a1 = 1.0 / (1.0 + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;
        float m0 = 0.0;
        float m1 = 0.0;
        float m2 = 1.0;
        return SVFCoeffs{a1, a2, a3, m0, m1, m2, k};
      }
      
      inline auto bpf(float w, float q) -> SVFCoeffs {
        float g = tanf(w/2.0);
        float k = 1.0/q;
        float a1 = 1.0 / (1.0 + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;
        float m0 = 0.0;
        float m1 = 1.0;
        float m2 = 0.0;
        return SVFCoeffs{a1, a2, a3, m0, m1, m2, k};
      }

      inline auto hpf(float w, float q) -> SVFCoeffs {
        float g = tanf(w/2.0);
        float k = 1.0/q;
        float a1 = 1.0 / (1.0 + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;
        float m0 = 1.0;
        float m1 = -k;
        float m2 = -1.0;
        return SVFCoeffs{a1, a2, a3, m0, m1, m2, k};
      }

      inline auto peq(float w, float q, float gain) -> SVFCoeffs {
        float a = powf(10.0, gain / 40.0);
        float g = tanf(w/2.0) * sqrt(gain);
        float k = 1.0/q;
        float a1 = 1.0 / (1.0 + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;
        float m0 = 1.0;
        float m1 = k * (a * a - 1.0);
        float m2 = 0.0;
        return SVFCoeffs{a1, a2, a3, m0, m1, m2, k};
      }

      inline auto notch(float w, float q) -> SVFCoeffs {
        float g = tanf(w/2.0);
        float k = 1.0/q;
        float a1 = 1.0 / (1.0 + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;
        float m0 = 1.0;
        float m1 = -k;
        float m2 = 0.0;
        return SVFCoeffs{a1, a2, a3, m0, m1, m2, k};
      }

      inline auto low_shelf(float w, float q, float gain) -> SVFCoeffs { 
        float a = powf(10.0, gain / 40.0);
        float g = tanf(w/2.0) * sqrt(gain);
        float k = 1.0/q;
        float a1 = 1.0 / (1.0 + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;
        float m0 = 1.0;
        float m1 = k * (a - 1.0);
        float m2 = a * a - 1.0;
        return SVFCoeffs{a1, a2, a3, m0, m1, m2, k};
      }

      inline auto high_shelf(float w, float q, float gain) -> SVFCoeffs { 
        float a = powf(10.0, gain / 40.0);
        float g = tanf(w/2.0) * sqrt(gain);
        float k = 1.0/q;
        float a1 = 1.0 / (1.0 + g * (g + k));
        float a2 = g * a1;
        float a3 = g * a2;
        float m0 = a * a;
        float m1 = k * (1.0 - a) * a;
        float m2 = 1.0 - a * a;
        return SVFCoeffs{a1, a2, a3, m0, m1, m2, k};
      }
    };

    class SVFilter {

      struct M {
        float ic1eq = 0.f; // integrator capacitor 1 (equalized)
        float ic2eq = 0.f; // integrator capacitor 2 (equalized)
        SVFCoeffs coeffs;
      } self;
      explicit SVFilter(M self ): self(std::move(self)){}
      public:

      static SVFilter init() {{
        return SVFilter(M{
          0.f,
          0.f,
          SVFCoeffs()
        });
      }}

      SVFilter() {};
      float process(float sample) {
        // v0 is sample
        //
        // v3 = v0 - self.ic2eq
        // v1 = a1 * self.ic1eq + a2 * v3
        // v2 = self.ic2eq + a2 * self.ic1eq + a3 * v3
        // self.ic1eq = 2 * v1 - self.ic1eq
        // self.ic2eq = 2 * v2 - self.ic2eq
        //
        // output = m0 * v0 + m1 * v1 + m2 * v2

        float v3 = sample - self.ic2eq;
        float v1 = self.coeffs.a1 * self.ic1eq + self.coeffs.a2 * v3;
        float v2 = self.ic2eq + self.coeffs.a2 * self.ic1eq + self.coeffs.a3 * v3;
        self.ic1eq = 2.0 * v1 - self.ic1eq;
        self.ic2eq = 2.0 * v2 - self.ic2eq;
        return self.coeffs.m0 * sample + self.coeffs.m1 * v1 + self.coeffs.m2 * v2;
      }

      void update(SVFCoeffs coeffs) {
        self.coeffs = coeffs;
      };
    };
  }
}

#endif
