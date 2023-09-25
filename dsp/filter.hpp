#include "buffer.hpp"

namespace dspheaders {
  enum COMBTYPE {
    FIR,
    IIR
  };

  class Comb {
    protected:
      Buffer buffer;
      float previn = 0.00001f;
      float prevout = 0.00001f;
      float read(float readptr);
      void write(float sample);
      unsigned writeptr = 0;
      float readptr = 0.f;

    public: 
      // Feedback
      float playIIR(float sample, float feedback);
      float playIIR(float sample, float feedback, float mod);
      // Feedforward
      float playFIR(float sample, float amp);
      float playFIR(float sample, float amp, float mod);

      float play(float sample, float feedback, COMBTYPE type);
      float play(float sample, float feedback, float mod, COMBTYPE type);
      Comb(
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
      );
  };

  class Allpass : public Comb {
    public:
      float play(float sample, float feedback);
      float play(float sample, float feedback, float mod);
      Allpass(
        unsigned offset,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
  };
}


