#pragma once

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

      // static float bilinear(float position, float* table, int tableLength);

      static float cubic(float position, float* table, int tableLength);

      static float hermetic(float position, float* table, int tableLength);

  };
}
