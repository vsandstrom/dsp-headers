#pragma once
#include <utility>


namespace dspheaders {
  class Impulse {
    /*
     * Trigger generator with fixed duration
     */
    protected:
      struct M {
        float samplerate;
        size_t dur = 0;
        size_t count = 0;
      } m;
      explicit Impulse(M m) : m(std::move(m)) {}

    public: 
      static Impulse init(float samplerate) {
        return Impulse(M{
          .samplerate = samplerate,
          .count = 0
        });
      }

      float play(float dur) {
        float out = 0.f;
        if (m.count >= m.dur) {
          m.count = 0;
          m.dur = m.samplerate * dur;
          out = 1.f;
        } else {
          m.count++;
        }
        return out;
      }

      // Impulse(float samplerate);
  };

  class Dust: public Impulse {
    /*
     * Trigger generator with random duration.
     * Conceptual clone of the Dust trigger generator from
     * SuperCollider 3
     */
    private:
      explicit Dust(M m) : Impulse(m) {}
    public:
      static Dust init(float samplerate) {
        return Dust(M{
          .samplerate = samplerate,
          .count = 0
        });
      }

      float play(float dur) {
        if (m.count >= m.dur) {
          float rng = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.f;
          m.dur = rng * dur * m.samplerate;
          m.count = 0;
          return 1.f;
        }
        m.count++;
        return 0.f;
      }

      // Dust(float samplerate);

  };
}
