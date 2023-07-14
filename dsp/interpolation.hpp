#ifndef PI 
  #define PI 
  const float pi = 3.14159265358979323846f;
#endif

#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

namespace dspheaders {
  enum INTERPOLATION {
    LINEAR,
    COSINE,
    CUBIC,
    HERMITE
  };

  class Interpolation {
    public:
      static float linear(float position, float* table);

      static float cosine(float position, float* table);

      static float bilinear(float position, float* table, int tableLength);

      static float cubic(float position, float* table, int tableLength);

      static float hermetic(float position, float* table, int tableLength);

  };
}
#endif
