#include <utility>

namespace dspheaders {
  class Wavetable {
    struct M {
      float position = 0.f;
      float samplerate = 0.f;
      float sr_recip = 0.f;
    } m;

    explicit Wavetable(M m) : m(std::move(m)) {}

    public:
    static Wavetable init(float samplerate) {
      return Wavetable(M{
          .position = 0.f,
          .samplerate = samplerate,
          .sr_recip = 1.f/samplerate
        }
      );
    }

    template <unsigned SIZE, float(*interpolate) (float, float*, unsigned)>
    float play(float* table, float frequency, float phase) {
      if (frequency > m.samplerate * 0.5f) return 0;
      float len = static_cast<float>(SIZE);
      m.position += (len * m.sr_recip * frequency) + (phase * len);
      while (m.position <  0.f) m.position += len;
      while (m.position >= len) m.position -= len;
      return interpolate(m.position, table, SIZE);
    };

    void setSamplerate(float samplerate) {
      m.samplerate = samplerate;
      m.sr_recip = 1.0 / samplerate;
    }
  };
}
