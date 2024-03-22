#include "dsp/dsp.h"
#include <cstdio>
#include "dsp/envelope.hpp"
#include "dsp/interpolation.hpp"


const float envpoints[] = {0, 1, 0};
const float envtimes[] = {0.2, 0.7};


dspheaders::Envelope env = dspheaders::Envelope(envpoints, 3, envtimes, 2, 48000, dspheaders::interpolation::linear);



int main() {
  env.repr();
}
