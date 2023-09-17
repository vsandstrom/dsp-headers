#include "buffer.hpp"

namespace dspheaders {
  class Comb {
    private:
      Buffer buffer;
      float read(unsigned readptr);
      void write(float sample);
      void write(float sample, float mod);
      float writeptr = 0;
      unsigned readptr = 0.f;

    public: 
      float play(float sample, float feedback);
      float play(float sample, float feedback, float mod);
      Comb(
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
      );
  };
  
}
