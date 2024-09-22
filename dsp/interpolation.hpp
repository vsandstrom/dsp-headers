#include <cstdlib>

#pragma once

namespace dspheaders {
  enum INTERPOLATION {
    LINEAR,
    COSINE,
    CUBIC,
    HERMITE
  };

  namespace interpolation {
    float none(float position, float* table, size_t tablelength);
    float linear(float position, float* table, size_t tablelength);
    float cosine(float position, float* table, size_t tablelength);
    float cubic(float position, float* table, size_t tableLength);
    float hermetic(float position, float* table, size_t tableLength);
    float slope(float a, float b);

  }
  

}


