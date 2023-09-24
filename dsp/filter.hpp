#include "buffer.hpp"

namespace dspheaders {
    class Comb {
      protected:
        Buffer* buffer;
        float previn = 0.00001f;
        float prevout = 0.00001f;
        unsigned writeptr = 0;
        float readptr = 0.f;

      public: 
        float read(float readptr);
        void write(float sample);
        virtual float play(float sample, float feedback);
        virtual float play(float sample, float feedback, float mod);
        Comb(
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
        );

        // When using both CombIIR and CombFIR with the same buffer
        Comb(
          Buffer* buffer,
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
        );
    };

///////////////////////////////////////////////////////////////////////////////
/// COMB IIR
///////////////////////////////////////////////////////////////////////////////

    class CombIIR : public Comb {
      // Based on:
      // https://ccrma.stanford.edu/~jos/pasp/Feedback_Comb_Filters.html
      //
      //              -----------------------> y(n)
      //             |     ----------         
      // x(n) ---(+)---> |   z[-M]  | ---
      //         |       -----------    |
      //         ---------(- aM)--------
      //
      private:
        float previn = 0.00001f;
        float prevout = 0.00001f;
        unsigned writeptr = 0;
        float readptr = 0.f;

        float read(float readptr);
        void write(float sample);

      public: 
        float play(float sample, float feedback);
        float play(float sample, float feedback, float mod);

        CombIIR (
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
        );

        CombIIR (
          Buffer* buffer,
          unsigned offset,
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
        );
    };

///////////////////////////////////////////////////////////////////////////////
/// COMB FIR
///////////////////////////////////////////////////////////////////////////////

    class CombFIR : public Comb {
      // Based on:
      // https://ccrma.stanford.edu/~jos/pasp/Feedforward_Comb_Filters.html
      //
      //    -----------( * b0 )---------------
      //   |         ----------              |
      // x(n)  --> |   z[-M]  | -( * bM)--> (+) --> y(n)
      //           -----------
      //
      private:
        float previn = 0.00001f;
        float prevout = 0.00001f;
        unsigned writeptr = 0;
        float readptr = 0.f;

        float read(float readptr);
        void write(float sample);

      public: 
        float play(float sample, float feedback);
        float play(float sample, float feedback, float mod);
        
        CombFIR (
            unsigned offset,
            unsigned samplerate,
            float (*interpolate)(float, float*, unsigned)
        );

        CombFIR (
            Buffer* buffer,
            unsigned offset,
            unsigned samplerate,
            float (*interpolate)(float, float*, unsigned)
        );
    };

///////////////
/// ALLPASS ///
///////////////

  // Based on:
  // https://ccrma.stanford.edu/~jos/pasp/Allpass_Two_Combs.html
  //
  //               ----( * b0 )------------
  //              |   ----------          |
  // x(n) ---(+)---> |   z[-M]  | -----> (+) --> y(n)
  //         |       -----------    |
  //         -------( * -aM )-------
    class Allpass {
      private:
        Buffer buffer;
        CombIIR iir;
        CombFIR fir;

      public:
        float play(float sample, float amount);
        float play(float sample, float amount, float mod);

        Allpass(
          unsigned offset, 
          unsigned samplerate,
          float (*interpolate)(float, float*, unsigned)
        );
    };
}
