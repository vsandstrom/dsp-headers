#include <cstddef>
#include <utility>

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
    template<size_t SIZE, size_t WIDTH, float(*interpolate)(float, float*, size_t)>
    float play(float** tables, float frequency, float position, float phase) {
      D({
        for (int i = 0; i < WIDTH; i++) {
          assert(tables[i] != nullptr && "table is not initialized");
        }
      })
      if (frequency > m.samplerate * 0.5) return 0.0;
      float sig = 0.f;
      float len = static_cast<float>(SIZE);
      float wid = static_cast<float>(WIDTH);

      if (position >= 1.f) position = 0.999999999999;
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
}
