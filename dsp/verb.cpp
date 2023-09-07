#include "dsp.h"
#include "interpolation.hpp"
#include "verb.hpp"
#include "delay.hpp"

using namespace dspheaders;

Verb::Verb (
    unsigned samplerate,
    float feedback,
    float (*interpolate)(float, float*, unsigned)
  ): 
  feedback(feedback),
  samplerate(samplerate)
{
  Delay lines[4] = {
    Delay(samplerate, 8.f, interpolate),
    Delay(samplerate, 8.f, interpolate),
    Delay(samplerate, 8.f, interpolate),
    Delay(samplerate, 8.f, interpolate),
  };
  delaylines = lines;
};

void Verb::write(float sample) {
  int n, m;
  for (n = 0; n < 4; n++){
    delaylines[n].write(sample, readptr + i[n]);
  }
}

float Verb::read() {
  float out = 0.0f;
  int n, m;

  for (n = 0; n < 4; n++) {
    for (m = 0; m < 4; m++) {
      out += k[m] * delaylines[m].read((int)(readptr + (float)i[n]));
    }
  }
  readptr++;
  return out;
}

float Verb::play(float sample) {
  write(sample);
  return read();
}
      

