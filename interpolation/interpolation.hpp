#ifndef PI
const float pi = 3.14159265358979323846;
#define PI
#endif


class Interpolation {
  public:
    static float linear(float position, float* table);
    static float cosine(float position, float* table);
};
