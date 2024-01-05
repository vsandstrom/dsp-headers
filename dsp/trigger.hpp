#pragma once
namespace dspheaders {
  class Impulse {
    /*
     * 
     * Trigger generator with fixed duration
     *
     */
    protected:
      float samplerate;
      float m_dur;
      int m_count = 0;

    public: 
      float play();
      float play(float interval);

      Impulse(float interval, float samplerate);
  };

  class Dust: public Impulse {
    /*
     *
     * Trigger generator with random duration.
     * Conceptual clone of the Dust trigger generator from
     * SuperCollider 3
     *
     */
    private:
      float relDur;

    public:
      float play();
      float play(float interval);
      Dust(float interval, float samplerate);

  };
}
