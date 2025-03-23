#pragma once
#ifndef BIQUAD_HPP
#define BIQUAD_HPP

#include <cstddef>
#include <cmath>
#include <utility>

namespace dspheaders {
  struct BiquadCoeffs {
    // feedback coeffs
    float a1 = 0.f, a2 = 0.f;
    // feedforward coeffs
    float b0 = 0.f, b1 = 0.f, b2 = 0.f;

    auto lpf(float w, float q) {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      float a1 = (-2.0 * cosf(w)) / a0 ;
      float a2 = (1.0 - alpha) / a0;

      float b1 = (1.0 - cosf(w)) / a0;
      float b0 = b1 / 2.0 / a0;
      float b2 = b0;
      return BiquadCoeffs{.a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }
 
    inline auto bpf(float w, float q) {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      float a1 = (-2.0 * cosf(w)) / a0 ;
      float a2 = (1.0 - alpha) / a0;

      float b0 = alpha / a0;
      float b1 = 0.0;
      float b2 = -alpha / a0;
      return BiquadCoeffs{.a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }

    inline auto hpf(float w, float q) {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      float a1 = (-2.0 * cosf(w)) / a0 ;
      float a2 = (1.0 - alpha) / a0;

      float b0 = (1.0 + cosf(w)) / 2.0 / a0;
      float b1 = -(b0 * 2.0);
      float b2 = b0;
      return BiquadCoeffs{.a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }
  
    inline auto notch(float w, float q) {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      float a1 = (-2.0 * cosf(w)) / a0 ;
      float a2 = (1.0 - alpha) / a0;

      float b0 = 1.0 / a0;
      float b1 = a1;
      float b2 = b0;
      return BiquadCoeffs{.a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }

    inline auto notch(float w, float q, float gain) {
      float alpha = sinf(w) / (2.0 * q);
      float a = powf(10.0, gain/40.0);
      float a0 = 1.0 + alpha;
      float a1 = (-2.0 * cosf(w)) / a0 ;
      float a2 = (1.0 - alpha) / a0;
      float b0 = (1.0 + alpha) * a / a0;
      float b1 = a1;
      float b2 = (1.0 - alpha) * a / a0;
      return BiquadCoeffs{.a1 = a1, .a2 = a2, .b0 = b0, .b1 = b1, .b2 = b2};
    }
  };

///
///                 biquad filter
///
///            
///             
///  in ─> ( + )───────( * a0 )────╖────────────────( + )──> out
///          Λ                     ║                  Λ
///          ╙──( - )──( * a1 )<─|n-1|->( * b1 )───────
///          Λ                     ║                  Λ
///          ╙──( - )──( * a2 )<─|n-2|->( * b2 )───────
///
///
///
  // TWO POLE STATE VARIABLE BIQUAD FILTER
  class Biquad {
    protected:
    struct M {
      BiquadCoeffs c;
      float x1;
      float x2;
      float y1;
      float y2;
    } self;

    explicit Biquad(M self): self(std::move(self)){}
    private:
    float inline calc_result(float input) {
      float output = self.c.b0 * input 
                   + self.c.b1 * self.x1
                   + self.c.b2 * self.x2
                   - self.c.a1 * self.y1
                   - self.c.a2 * self.y2;
      return output;
    }

    void inline update_values(float input, float output) {
      self.x2 = self.x1;
      self.x1 = input;
      self.y2 = self.y1;
      self.y1 = output;
    }

    public:

    Biquad(){}
    static Biquad init(BiquadCoeffs coeffs) {
      return Biquad(
        M{
        .c = coeffs,
        .x1 = 0.0, .x2 = 0.0,
        .y1 = 0.0,
        .y2 = 0.0,
        }
      );
    }
    // Supply custom feedback coeffs, must be *at least* 3, only 3 values will 
    // be read if longer

    void update(BiquadCoeffs coeffs) {
      self.c = coeffs;
    }

    float process(float input, float initial_amplitude) {
      float output = calc_result(input);
      update_values(input, output);
      return output;
    }

    // angular frequency = 'w'
    // w = 2pi * (center frequency / samplerate)
    //
    // quality factor (bandwidth) = 'q'
    // BW = center frequency / q
    //
    // /=============|=====|======\
    // | center freq |  Q  |  BW  |
    // |=============|=====|======|
    // |    1000     |   1 | 1000 |
    // |    1000     |   2 |  500 |
    // |    1000     |  10 |  100 |
    // |    1000     | 0.5 | 2000 |
    // >=============|=====|======<
    inline auto calc_lpf(float w, float q){
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      self.c.a1 = (-2.0 * cosf(w)) / a0 ;
      self.c.a2 = (1.0 - alpha) / a0;

      self.c.b1 = (1.0 - cosf(w)) / a0;
      self.c.b0 = self.c.b1 / 2.0 / a0;
      self.c.b2 = self.c.b0;
    }
      
    // angular frequency = 'w'
    // w = 2pi * (center frequency / samplerate)
    //
    // quality factor (bandwidth) = 'q'
    // BW = center frequency / q
    //
    // /=============|=====|======\
    // | center freq |  Q  |  BW  |
    // |=============|=====|======|
    // |    1000     |   1 | 1000 |
    // |    1000     |   2 |  500 |
    // |    1000     |  10 |  100 |
    // |    1000     | 0.5 | 2000 |
    // >=============|=====|======<
    inline void calc_bpf(float w, float q) {
      float alpha = sinf(w) / (2.0 * q);
      
      float a0 = 1.0 + alpha;
      self.c.a1 = (-2.0 * cosf(w)) / a0 ;
      self.c.a2 = (1.0 - alpha) / a0;

      self.c.b0 = alpha / a0;
      self.c.b1 = 0.0;
      self.c.b2 = -alpha / a0;
    }

    // angular frequency = 'w'
    // w = 2pi * (center frequency / samplerate)
    //
    // quality factor (bandwidth) = 'q'
    // BW = center frequency / q
    //
    // /=============|=====|======\
    // | center freq |  Q  |  BW  |
    // |=============|=====|======|
    // |    1000     |   1 | 1000 |
    // |    1000     |   2 |  500 |
    // |    1000     |  10 |  100 |
    // |    1000     | 0.5 | 2000 |
    // >=============|=====|======<
    inline void calc_hpf(float w, float q) {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      self.c.a1 = (-2.0 * cosf(w)) / a0;
      self.c.a2 = 1.0 - alpha / a0;

      self.c.b0 = (1.0 + cosf(w)) / 2.0 / a0;
      self.c.b1 = -(self.c.b0 * 2.0);
      self.c.b2 = self.c.b0;
    }

    // angular frequency = 'w'
    // w = 2pi * (center frequency / samplerate)
    //
    // quality factor (bandwidth) = 'q'
    // BW = center frequency / q
    //
    // /=============|=====|======\
    // | center freq |  Q  |  BW  |
    // |=============|=====|======|
    // |    1000     |   1 | 1000 |
    // |    1000     |   2 |  500 |
    // |    1000     |  10 |  100 |
    // |    1000     | 0.5 | 2000 |
    // >=============|=====|======<
    inline void calc_notch(float w, float q) {
      float alpha = sinf(w) / (2.0 * q);
      float a0 = 1.0 + alpha;
      self.c.a1 = -2.0 * cosf(w) / a0;
      self.c.a2 = (1.0 - alpha) / a0;

      self.c.b0 = 1.0 / a0;
      self.c.b1 = self.c.a1;
      self.c.b2 = self.c.b0;
    }

  };

  // FOUR POLE BIQUAD SVF
  class Biquad4 : public Biquad {
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
        float output = self.c.b0*input 
                     + self.c.b1*x1_1 
                     + self.c.b2*x1_2 
                     - self.c.a1*y1_1 
                     - self.c.a2*y1_2;
        x1_2 = x1_1;
        x1_1 = input;
        y1_2 = y1_1;
        y1_1 = output;
        
        //step 2
        output = self.c.b0*output 
               + self.c.b1*x2_1 
               + self.c.b2*x2_2
               - self.c.a1*y2_1 
               - self.c.a2*y2_2;
        x2_2 = x2_1;
        x2_1 = y1_1;
        y2_2 = y2_1;
        y2_1 = output;

        return output;
      }
  };


  // EIGHT POLE BIQUAD SVF
  class Biquad8 : public Biquad {
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
        float output = 0.f;
        // step 1
        output = self.c.b0*input
               + self.c.b1*x1_1
               + self.c.b2*x1_2 
               - self.c.a1*y1_1 
               - self.c.a2*y1_2;
        x1_2 = x1_1;
        x1_1 = input;
        y1_2 = y1_1;
        y1_1 = output;
        
        //step 2
        output = self.c.b0*output 
               + self.c.b1*x2_1
               + self.c.b2*x2_2 
               - self.c.a1*y2_1 
               - self.c.a2*y2_2;
        x2_2 = x2_1;
        x2_1 = y1_1;
        y2_2 = y2_1;
        y2_1 = output;

        //step 3
        output = self.c.b0*output 
               + self.c.b1*x3_1
               + self.c.b2*x3_2 
               - self.c.a1*y3_1 
               - self.c.a2*y3_2;
        x3_2 = x3_1;
        x3_1 = y2_1;
        y3_2 = y3_1;
        y3_1 = output;

        //step 4
        output = self.c.b0*output 
               + self.c.b1*x4_1
               + self.c.b2*x4_2 
               - self.c.a1*y4_1 
               - self.c.a2*y4_2;
        x4_2 = x4_1;
        x4_1 = y3_1;
        y4_2 = y4_1;
        y4_1 = output;
        return output;
      }
  };
}

#endif
