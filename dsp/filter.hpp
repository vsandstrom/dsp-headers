#pragma once
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
  enum COMBTYPE {
    FIR,
    IIR
  };

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
          .damp = 0.f,
          .previous = 0.f, 
          .feedforward = feedforward,
          .feedback = feedback,
          .position = 0,
          .delay = 0,
          .previous_in = 0.f,
          .previous_out = 0.f
        });
      }
      // Feedback
      inline void setDamp(float damp) {
        m.damp = damp;
      }

      inline unsigned getBufferLength() {
        return m.bufferlength;
      }

      float process(float sample) {
        float delayed = m.buffer[m.position];
        float dc_blocked = sample - m.previous_in + 0.995 * m.previous_out;

        m.previous_in = sample;
        m.previous_out = dc_blocked;

        m.previous = delayed * (1.f * m.damp) + m.previous * m.damp;
        float fb = dc_blocked - m.feedback * m.previous;
        m.buffer[m.position] = fb;
        m.position = m.position + 1;
        // handle maximum size of delay line
        while (m.position >= m.bufferlength) m.position -= m.delay;
        return m.feedforward * fb + delayed;
      }
  };

  struct BiquadCoeffs {
    float a1 = 0.f;
    float a2 = 0.f;
    float b0 = 0.f;
    float b1 = 0.f;
    float b2 = 0.f;
  };

  class Biquad {
    // feedforward coeffs
    struct M {
      float b0;
      float b1; 
      float b2;
    // feedback coeffs
      float a1;
      float a2;
    
      float x1;
      float x2;
      float y1;
      float y2;
    }m;

    public:
    Biquad(){}
    // Supply custom feedback coeffs, must be *at least* 3, only 3 values will 
    // be read if longer

    void set_coeffs(BiquadCoeffs* coeffs) {
      m.a1 = coeffs->a1;
      m.a2 = coeffs->a2;
      m.b0 = coeffs->b0;
      m.b1 = coeffs->b1;
      m.b2 = coeffs->b2;
    }

    float process(float input, float initial_amplitude) {
      float output = m.b0 * input 
                     + m.b1 * m.x1
                     + m.b2 * m.x2
                     - m.a1 * m.y1
                     - m.a2 * m.y2;

      m.x2 = m.x1;
      m.x1 = input;
      m.y2 = m.y1;
      m.y1 = output;
      return output;
    }


    inline auto calc_lpf(float w, float q) -> BiquadCoeffs {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      float a1 = (-1.0 * cosf(w)) / a0 ;
      float a2 = (1.0 - alpha) / a0;

      float b1 = (1.0 - cosf(w)) / a0;
      float b0 = b1 / 2.0 / a0;
      float b2 = b0;
      return BiquadCoeffs{a1, a2, b0, b1, b2};
    }
      
    inline auto calc_bpf(float w, float q) -> BiquadCoeffs {
      float alpha = sinf(w) / (2.0 * q);
      
      float a0 = 1.0 + alpha;
      float a1 = (-1.0 * cosf(w)) / a0;
      float a2 = (1.0 - alpha) / a0;

      float b0 = alpha / a0;
      float b1 = 0.0;
      float b2 = -alpha / a0;
      return BiquadCoeffs{a1, a2, b0, b1, b2};
    }

    inline auto calc_hpf(float w, float q) -> BiquadCoeffs {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      float a1 = -1.0 * cosf(w) / a0;
      float a2 = 1.0 - alpha / a0;

      float b0 = (1.0 + cosf(w)) / 2.0 / a0;
      float b1 = -(b0 * 2.0);
      float b2 = b0;
      return BiquadCoeffs{a1, a2, b0, b1, b2};
    }

    inline auto calc_notch(float w, float q) -> BiquadCoeffs {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      float a1 = -2.0 * cosf(w) / a0;
      float a2 = (1.0 - alpha) / a0;

      float b0 = 1.0 / a0;
      float b1 = a1;
      float b2 = b0;
      return BiquadCoeffs{a1, a2, b0, b1, b2};
    }
  };
}

#endif
