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

  // TWO POLE STATE VARIABLE BIQUAD FILTER
  class _Biquad {
    private:
      // 2-sample delayline on input side
      float x1 = 0.f, x2 = 0.f;
      // 2-sample delayline on output side
      float y1 = 0.f, y2 = 0.f;

    protected:
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
      _Biquad(){}

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

        a0 = 1 + alpha;             //  1 + alpha
        a1 = -2 * cos(omega) / a0;  // -2 * cos(omega)
        a2 = (1 - alpha) / a0;      //  1 - alpha
                                  
        // See order:
        b1 = (1 - cos(omega)) / a0; //  1-cos(omega)
        b0 = b1 / 2;                // (1-cos(omega)) / 2
        b2 = b0;                    // (1-cos(omega)) / 2
        // normalizeCoeffs();
      }

      // Band Pass Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      constexpr void calcBPF(float omega, float q) {
        float alpha = sin(omega) / (2 * q);
        a0 = 1 + alpha;             //  1 + alpha
        a1 = -2 * cos(omega) / a0;  // -2 * cos(omega)
        a2 = (1 - alpha) / a0;      //  1 - alpha

        b0 = alpha / a0;            // alpha
        b1 = 0;                     // 0
        b2 = -alpha / a0;           // -alpha

        // normalizeCoeffs();
      }

      // Band Pass Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      // float gain = Gain in dB
      constexpr void calcPEQ(float omega, float q, float gain) {
        float alpha = sin(omega) / (2 * q);
        float A = powf(10, gain/40.f);

        a0 = (1 + alpha) / A;       //  1 + alpha
        a1 = -2 * cos(omega) / a0;  // -2 * cos(omega)
        a2 = (1 - alpha) / A / a0;  //  1 - alpha / A
        
        b0 = (1 + alpha) * A / a0;  // 1 + alpha * A
        b1 = a1;                    // -2 * cos(omega)
        b2 = (1 - alpha) * A / a0;  // 1 - alpha * A
        // normalizeCoeffs();
      }

      // Notch Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      constexpr void calcNotch(float omega, float q) {
        float alpha = sin(omega) / (2 * q);
        a0 = 1 + alpha;             //  1 + alpha
        a1 = -2 * cos(omega) / a0;  // -2 * cos(omega)
        a2 = (1 - alpha) / a0;      //  1 - alpha

        b0 = 1 / a0;                //  1
        b1 = a1;                    // -2 * cos(omega)
        b2 = b0;                    //  1
        // normalizeCoeffs();
      }

      // High Pass Filter
      // float omega = Angular velocity: 2PI * Frequency / Samplerate
      // float q = Frequency / Bandwidth in Hz.
      constexpr void calcHPF(float omega, float q) {
        float alpha = sin(omega) / (2 * q);
        a0 = 1 + alpha;                 //  1 + alpha
        a1 = -2 * cos(omega) / a0;      // -2 * cos(omega)
        a2 = (1 - alpha) / a0;          //  1 - alpha

        b0 = (1 + cos(omega)) / 2 / a0; //  (1 + cos(omega)) / 2
        b1 = -(b0 * 2);                 // -(1 + cos(omega))
        b2 = b0;                        //  (1 + cos(omega)) / 2
        // normalizeCoeffs();
      }
  };

  // FOUR POLE BIQUAD SVF
  class Biquad4 : public _Biquad {
    private:
      // delayline for 1st biquad section.
      float x1_1 = 0.f, x1_2 = 0.f;
      float y1_1 = 0.f, y1_2 = 0.f;

      // delayline for 2nd biquad section.
      float x2_1 = 0.f, x2_2 = 0.f;
      float y2_1 = 0.f, y2_2 = 0.f;
    
    public:
      Biquad4(){};

      inline float process(float input) {
        // step 1
        float output = b0*input + b1*x1_1 + b2*x1_2 - a1*y1_1 - a2*y1_2;
        x1_2 = x1_1;
        x1_1 = input;
        y1_2 = y1_1;
        y1_1 = output;
        
        //step 2
        output = b0*output + b1*x2_1 + b2*x2_2 - a1*y2_1 - a2*y2_2;
        x2_2 = x2_1;
        x2_1 = y1_1;
        y2_2 = y2_1;
        y2_1 = output;

        return output;
      }
  };


  // EIGHT POLE BIQUAD SVF
  class Biquad8 : public _Biquad {
    private:
      // delayline for 1st biquad section.
      float x1_1 = 0.f, x1_2 = 0.f;
      float y1_1 = 0.f, y1_2 = 0.f;

      // delayline for 2nd biquad section.
      float x2_1 = 0.f, x2_2 = 0.f;
      float y2_1 = 0.f, y2_2 = 0.f;

      // delayline for 3rd biquad section.
      float x3_1 = 0.f, x3_2 = 0.f;
      float y3_1 = 0.f, y3_2 = 0.f;

      // delayline for 4th biquad section.
      float x4_1 = 0.f, x4_2 = 0.f;
      float y4_1 = 0.f, y4_2 = 0.f;

    public:
      Biquad8(){};

      inline float process(float input) {
        // step 1
        float output = b0*input + b1*x1_1 + b2*x1_2 - a1*y1_1 - a2*y1_2;
        x1_2 = x1_1;
        x1_1 = input;
        y1_2 = y1_1;
        y1_1 = output;
        
        //step 2
        output = b0*output + b1*x2_1 + b2*x2_2 - a1*y2_1 - a2*y2_2;
        x2_2 = x2_1;
        x2_1 = y1_1;
        y2_2 = y2_1;
        y2_1 = output;

        //step 3
        output = b0*output + b1*x3_1 + b2*x3_2 - a1*y3_1 - a2*y3_2;
        x3_2 = x3_1;
        x3_1 = y2_1;
        y3_2 = y3_1;
        y3_1 = output;

        //step 4
        output = b0*output + b1*x4_1 + b2*x4_2 - a1*y4_1 - a2*y4_2;
        x4_2 = x4_1;
        x4_1 = y3_1;
        y4_2 = y4_1;
        y4_1 = output;
        return output;
      }
  };
}

#endif
