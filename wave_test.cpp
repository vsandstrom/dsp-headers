#include "waveshape.h"
#include <cstdio>
#include <cstring>


int main(int argc, char** argv) {
  unsigned tlen = 16;
  float* t = new float[tlen];

  if (!strcmp("sine", argv[1])) {
    sine(t, tlen);
  } else if ( !strcmp("saw", argv[1]) ) {
    saw(t, tlen);
  } else if (!strcmp("square", argv[1])) {
    square(t, tlen);

  } else if (!strcmp("triangle", argv[1])) {
    triangle(t, tlen);

  } else if (!strcmp("hanning", argv[1])) {
    hanning(t, tlen);
  } else if (!strcmp("env", argv[1])) {
    hanning(t, tlen);
  } else {
    printf("No such waveshape");
    return 1;
  }


  for (unsigned i = 0; i < tlen; ++i) {
    printf("%f\n", t[i]);
  }
  return 0;
}
