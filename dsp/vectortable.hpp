#ifndef VECOSC_HPP
#define VECOSC_HPP

#include <cstddef>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include "dsp.h"

#ifndef DEBUG
  #define D(x)  
#else 
  #include <assert.h>
  #define D(x) x
#endif

namespace dspheaders {
  class VectorOscillator {
    private:
    struct M {
      float position = 0.f;
      float samplerate = 0.f;
      float sr_recip = 0.f;
    } m; 

    explicit VectorOscillator(M m) : m(std::move(m)) {}

    public:
    VectorOscillator(){}

    static VectorOscillator init(float samplerate) {
      return VectorOscillator(M{
          .position = 0.f,
          .samplerate = samplerate,
          .sr_recip = 1.f / samplerate
        }
      );
    }

    /*! SIZE determines the size of the wavetables used, 
     * WIDTH the number of wavetables used,
     * interpolate is a function pointer to an interpolation algorithm,
     * with a certain signature, see interpolation.hpp.
     * `tables` is a 2 dimensional array of floats:
     * `float[SIZE][WIDTH]` or `float**`.
     */
    template<size_t SIZE, size_t WIDTH, float(*interpolate)(const float, const float* const, const size_t)>
    float play(const float** tables, float frequency, float position, float phase) {
      D({
        for (int i = 0; i < WIDTH; i++) {
          assert(tables[i] != nullptr && "table is not initialized");
        }
      })
      if (frequency > m.samplerate * 0.5) return 0.0;
      float sig = 0.f;
      float len = static_cast<float>(SIZE);
      float wid = static_cast<float>(WIDTH);

      position = clamp(position, 0, 0.999999999999);
      position *= wid - 1.f;
      unsigned table1 = static_cast<size_t>(position) % WIDTH;
      unsigned table2 = (table1 + 1) % WIDTH;
      float weight = position - size_t(position);

      sig += interpolate(m.position, tables[table1], SIZE) * (1.f - weight);
      sig += interpolate(m.position, tables[table2], SIZE) * weight;

      m.position += len * m.sr_recip * frequency + (phase * len);
      while (m.position <  0.f) m.position += len;
      while (m.position >= len) m.position -= len;
      return sig;
    }

    void setSamplerate(float samplerate) {
      m.samplerate = samplerate;
      m.sr_recip = 1.f / samplerate;
    }

    void resetPhase() {
      m.position = 0.f;
    }
  };
  

// Inline Linear interpolation
  class VectorOscillatorLinear {
    private:
    struct M {
      float position = 0.f;
      float samplerate = 0.f;
      float sr_recip = 0.f;
    } m; 

    explicit VectorOscillatorLinear(M m) : m(std::move(m)) {}

    public:
    VectorOscillatorLinear(){}

    static VectorOscillatorLinear init(float samplerate) {
      return VectorOscillatorLinear(M{
          .position = 0.f,
          .samplerate = samplerate,
          .sr_recip = 1.f / samplerate
        }
      );
    }

    /*! SIZE determines the size of the wavetables used, 
     * WIDTH the number of wavetables used,
     * interpolate is a function pointer to an interpolation algorithm,
     * with a certain signature, see interpolation.hpp.
     * `tables` is a 2 dimensional array of floats:
     * `float[SIZE][WIDTH]` or `float**`.
     */
    template<size_t SIZE, size_t WIDTH>
    float play(const float** tables, float frequency, float position, float phase) {
      D({
        for (int i = 0; i < WIDTH; i++) {
          assert(tables[i] != nullptr && "table is not initialized");
        }
      })
      if (frequency > m.samplerate * 0.5) return 0.0;
      float len = static_cast<float>(SIZE);
      float wid = static_cast<float>(WIDTH);

      position = clamp(position, 0.f, 0.999999999999f);
      position *= wid - 1.f;
      size_t table1 = static_cast<size_t>(position) % WIDTH;
      size_t table2 = (table1 + 1) % WIDTH;

      size_t p1 = static_cast<size_t>(m.position);
      size_t p2 = p1 + 1;

      // a + x(b - a) + y((c - a) + x((b - a - d + c)))
      float x = m.position - p1;
      float y = position - static_cast<int>(position);

      float a = tables[table1][p1];
      float b = tables[table1][p2];
      float c = tables[table2][p1];
      float d = tables[table2][p2];

      float phaseLerp1 = lerp(x, a, b);
      float phaseLerp2 = lerp(x, c, d);
      float sig = phaseLerp1 + y * (phaseLerp2 - phaseLerp1);
      m.position += len * m.sr_recip * frequency + (phase * len);
      while (m.position <  0.f) m.position += len;
      while (m.position >= len) m.position -= len;
      return sig;
    }

    void setSamplerate(float samplerate) {
      m.samplerate = samplerate;
      m.sr_recip = 1.f / samplerate;
    }
    
    void resetPhase() {
      m.position = 0.f;
    }

    inline float lerp(float x, float a, float b) {
      return a + x * (b - a);
    }
  };
  

// Inline Linear interpolation
  // class VectorOscillatorLinear {
  //   private:
  //   struct M {
  //     float position = 0.f;
  //     float samplerate = 0.f;
  //     float sr_recip = 0.f;
  //   } m; 
  //
  //   explicit VectorOscillatorLinear(M m) : m(std::move(m)) {}
  //
  //   public:
  //   VectorOscillatorLinear(){}
  //
  //   static VectorOscillatorLinear init(float samplerate) {
  //     return VectorOscillatorLinear(M{
  //         .position = 0.f,
  //         .samplerate = samplerate,
  //         .sr_recip = 1.f / samplerate
  //       }
  //     );
  //   }
  //
  //   /*! SIZE determines the size of the wavetables used, 
  //    * WIDTH the number of wavetables used,
  //    * interpolate is a function pointer to an interpolation algorithm,
  //    * with a certain signature, see interpolation.hpp.
  //    * `tables` is a 2 dimensional array of floats:
  //    * `float[SIZE][WIDTH]` or `float**`.
  //    */
  //   template<size_t SIZE, size_t WIDTH>
  //   float play(float** tables, float frequency, float position, float phase) {
  //     D({
  //       for (int i = 0; i < WIDTH; i++) {
  //         assert(tables[i] != nullptr && "table is not initialized");
  //       }
  //     })
  //     if (frequency > m.samplerate * 0.5) return 0.0;
  //     float sig = 0.f;
  //     float len = static_cast<float>(SIZE);
  //     float wid = static_cast<float>(WIDTH);
  //
  //     position = clamp(position, 0.f, 0.999999999999f);
  //     position *= wid - 1.f;
  //     unsigned table1 = static_cast<size_t>(position) % WIDTH;
  //     unsigned table2 = (table1 + 1) % WIDTH;
  //     float weight = position - size_t(position);
  //
  //     int a = m.position;
  //     int b = a + 1;
  //     float w = m.position - a;
  //     sig += tables[table1][a] * (1.f - w) + tables[table1][b] * w * (1.f - weight);
  //     sig += tables[table2][a] * (1.f - w) + tables[table2][b] * w * weight;
  //     m.position += len * m.sr_recip * frequency + (phase * len);
  //     while (m.position <  0.f) m.position += len;
  //     while (m.position >= len) m.position -= len;
  //     return sig;
  //   }
  //
  //   void setSamplerate(float samplerate) {
  //     m.samplerate = samplerate;
  //     m.sr_recip = 1.f / samplerate;
  //   }
  // };
}

#endif
