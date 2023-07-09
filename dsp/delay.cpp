#include "dsp.h"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;
// A bunch of different constructors
Delay::Delay(unsigned int samplerate, float delay, unsigned int delay_taps): 
  delay(delay),
  samplerate(samplerate),
  buffer(Buffer(delay * 2, samplerate)) {
  initBuffer();
}

Delay::Delay(unsigned int samplerate, Buffer buffer, unsigned int delay_taps):
  samplerate(samplerate), 
  buffer(buffer) {
  initBuffer();
}

void Delay::write(float sample) {
  buffer.writeSample(sample, wrap(writeptr, buffer.bufferLength));
  writeptr++;
}

float Delay::read(float delaytime) {
  // Non-interpolating read function
  float output = 0.f;
  int tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    int tap = writeptr - (tapdelaytime * i);
    output += buffer.readSample(wrap(tap, buffer.bufferLength));
  }
  return output;
}

float Delay::readInterpolated(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    output += buffer.readInterpolatedSample(wrapf(tap, (float)buffer.bufferLength));
  }
  return output;


}

float Delay::play(float input, float delaytime, float wet, float feedback) {
  float output = readInterpolated(delaytime);
  // write the delay back to write head with feedback
  write(input + ( output * feedback ));
  // wet controls dry/wet balance of delay
  return output * wet;
}
