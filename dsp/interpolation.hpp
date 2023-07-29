#pragma once

namespace dspheaders {
  enum INTERPOLATION {
    LINEAR,
    COSINE,
    CUBIC,
    HERMITE
  };

  namespace interpolation {
    float none(float position, float* table, unsigned tablelength);
    float linear(float position, float* table, unsigned tablelength);
    float cosine(float position, float* table, unsigned tablelength);
    float cubic(float position, float* table, unsigned tableLength);
    float hermetic(float position, float* table, unsigned tableLength);

  }
}
