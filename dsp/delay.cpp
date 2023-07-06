#include "dsp.h"
#include "delay.hpp"
#include "interpolation.hpp"

/// Todo: 
/// Soft limit the range of what is used in the buffer, minimum 1 sample offset?
///
/// Adjustable speed through the buffer? should the taps also compensate? 
/// could that lead to some nice fluttering effects?

using namespace dspheaders;
// A bunch of different initializers
Delay::Delay(uint32_t samplerate, float delay, uint32_t delay_taps): 
  delay(delay),
  samplerate(samplerate),
  buffer(Buffer(delay * 2, samplerate)) {
  initBuffer();
}

Delay::Delay(uint32_t samplerate, Buffer buffer, uint32_t delay_taps):
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
  int timeInSamples = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    int tap = writeptr - (timeInSamples * i);
    output += buffer.readSample(wrap(tap, buffer.bufferLength));
  }
  return output;
}

float Delay::readInterpolated(float delaytime) {
  float output = 0.f;
  float timeInSamples = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (timeInSamples * i);
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
