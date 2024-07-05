#include "filter.hpp"
#include "dsp_math.h"

namespace dspheaders {
  template <unsigned T>
  class FilterBank {
    private:
      Biquad bank[T];
    protected:
      float amplitudes[T] {1.f};

    public:
      FilterBank(){}
      constexpr void init(float freq, float q, float samplerate) {
        for (int i = 0; i < T; i++) {
          float omega = freq * ((i * 2) + 1) * TAU / samplerate;
          bank[i].calcBPF(omega, q);
        }
      }

      float process(float input) {
        float output = 0.f;
        for (int i = 0; i < T; i++) {
          output += bank[i].process(input) * amplitudes[i];
        }
        return output;
      }

      void setAmp(size_t i, float amp) {
        amplitudes[i] = amp;
      }

      size_t size() {
        return T;
      }
  };

  template<unsigned T>
    class FilterBank4: public FilterBank<T> {
      private:
        Biquad4 bank[T];
      public:
        FilterBank4(){}
    };
  
  template<unsigned T>
    class FilterBank8: public FilterBank<T> {
      private:
        Biquad8 bank[T];
      public:
        FilterBank8(){}
    };
}
