#ifndef PI 
  #define PI 
  const float pi = 3.14159265358979323846f;
#endif

#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

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
};
#endif
