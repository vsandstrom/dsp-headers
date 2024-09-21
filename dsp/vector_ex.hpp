#include <cstddef>
#include <utility>
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
    static VectorOscillator init(float samplerate) {
      return VectorOscillator(M{
          .position = 0.f,
          .samplerate = samplerate,
          .sr_recip = 1.f / samplerate
        }
      );
    }

    template<unsigned SIZE, size_t WIDTH, float(*interpolate)(float, float*, unsigned)>
    float play(float** tables, float frequency, float position, float phase) {
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
  };
}
