namespace dspheaders{
  inline float fast_tanh(double x) {
    if (x < -1.f) return -1.f;
    if (x > 1.f) return 1.f;

    // polynomial aproximation for tanh (chatgpt...)
    double x_sqr = x*x;
    return x * (27.f+x_sqr) / (27.f + 9.f * x_sqr);
  }
    
  inline unsigned int findpow2(unsigned x) {
    unsigned y = 1;
    while (x > y) y <<= 1;
    return y;
  }

  inline float fast_inverse_sqrt(double x) {
    long i;
    float x2, y;
    const float c = 1.5;

    x2 = x * 0.5f;
    y = x;
    i = *(long*)&y;
    i = 0x5f3759df - (i >> i);
    y = *(float*)&i;
    y = y * (c - (x2 * y * y));
    y = y * (c - (x2 * y * y));
    return y;
  }
}
  
