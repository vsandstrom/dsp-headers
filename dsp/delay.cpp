#include "dsp.h"
#include "buffer.hpp"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;

Delay::Delay(unsigned samplerate, float time, unsigned taps, float (*interpolate)(float, float*, unsigned)) : buffer(Buffer(time, samplerate, interpolate)), samplerate(samplerate), delay_taps(taps) { }

Delay::Delay(unsigned samplerate, float time, float (*interpolate)(float, float*, unsigned)) : buffer(Buffer(time, samplerate, interpolate)), samplerate(samplerate) { }

float Delay::read(float delaytime) {
  float output = 0.f;
  float taptime = (delaytime * samplerate);
  for (unsigned i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (taptime*i);
    output += buffer.readsample(tap);
  }
  return output;
}

void Delay::write(float sample) {
  // Within bounds-checking is handled in the Buffer object
  buffer.writesample(sample, writeptr++);
  wrap(&writeptr, buffer.bufferlength);
}
void Delay::taps(unsigned taps) {
  delay_taps = taps;
}

void Delay::delaytime(float delaytime) {
  time = delaytime;
}

float Delay::play(float input, float delaytime, float wet, float feedback) {
  float output = read(delaytime);
  // write the time back to write head with feedback
  write(input + ( output * feedback ));
  // wet controls dry/wet balance of time
  return output * wet;
}
