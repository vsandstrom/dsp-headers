namespace dspheaders{
  inline float fast_tanh(double x) {
    if (x < -1.f) return -1.f;
    if (x > 1.f) return 1.f;

    // polynomial aproximation for tanh (chatgpt...)
    double x_sqr = x*x;
    return x * (27.f+x_sqr) / (27.f + 9.f * x_sqr);
  }
    
  inline unsigned int findpow2(unsigned size) {
    unsigned val = 1;
    while (size < val) val <<= 1;
    return val;
  }
}
  
