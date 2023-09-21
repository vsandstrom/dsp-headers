#include "buffer.hpp"

namespace dspheaders {
  namespace filter {
    class Comb {
      protected:
        Buffer buffer;
        float previn = 0.00001f;
        float prevout = 0.00001f;
        float read(unsigned readptr);
        void write(float sample);
        unsigned writeptr = 0;
        float readptr = 0.f;

      public: 
        virtual float play(float sample, float feedback);
        virtual float play(float sample, float feedback, float mod);
        Comb(
            unsigned offset,
            unsigned samplerate,
            float (*interpolate)(float, float*, unsigned)
        );
    };

///////////////////////////////////////////////////////////////////////////////
/// COMB IIR
///////////////////////////////////////////////////////////////////////////////

    class CombIIR : public Comb {
      CombIIR (
        unsigned offset,
        unsigned samplerate,
        float (*interpolate)(float, float*, unsigned)
      );
    };

///////////////////////////////////////////////////////////////////////////////
/// COMB FIR
///////////////////////////////////////////////////////////////////////////////

    class CombFIR : public Comb {
      float play(float sample, float feedback);
      float play(float sample, float feedback, float mod);

      CombFIR (
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
      );
    };



    class Allpass : public Comb {
        float play(float sample, float feedback);
        float play(float sample, float feedback, float mod);
        Allpass (
            unsigned offset,
            unsigned samplerate,
            float (*interpolate)(float, float*, unsigned)
        );

    };
  }
}
