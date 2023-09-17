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

  namespace writeinterpolation {
    // void none(float sample, float position, float* table, unsigned tablelength);
    void linear(float sample, float position, float* table, unsigned tablelength);
    // void cosine(float sample, float position, float* table, unsigned tablelength);
    // void cubic(float sample, float position, float* table, unsigned tableLength);
    // void hermetic(float sample, float position, float* table, unsigned tableLength);
  }
}


