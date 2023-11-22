
namespace dspheaders {
  class Impulse {
    /*
     * 
     * Trigger generator with fixed duration
     *
     */
    float dur;
    float samplerate;
    unsigned count = 0;

    public: 
      inline float play() {
        if (count >= dur * samplerate) {
          return 1.f;
          count = 0;
        }
        count++;
        return 0.f;
      }

      inline float play(float dur) {
        this -> dur = dur;
        if (count >= dur * samplerate) {
          return 1.f;
          count = 0;
        }
        count++;
        return 0.f;

      }

      Impulse(float dur, float samplerate)
      : dur(dur), samplerate(samplerate){}
  };

  class Dust {
    /*
     *
     * Trigger generator with random duration.
     * Conceptual clone of the Dust trigger generator from
     * SuperCollider 3
     *
     */

    // TODO

  };
}
