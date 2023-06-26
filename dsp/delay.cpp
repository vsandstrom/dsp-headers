#include "dsp.h"
#include "delay.hpp"
#include "interpolation.hpp"


#define LIMIT 16.f
// Hard limit for length of buffer

/// Todo: 
/// Soft limit the range of what is used in the buffer, minimum 1 sample offset?
///
/// Adjustable speed through the buffer? should the taps also compensate? 
/// could that lead to some nice fluttering effects?

using namespace dspheaders;

// A bunch of different initializers
Delay::Delay(uint32_t samplerate, float delay): delay(delay), samplerate(samplerate), buffer(Buffer<float>(delay * 2, samplerate)) {}

Delay::Delay(uint32_t samplerate, float delay, uint32_t delay_taps): delay(delay), samplerate(samplerate), buffer(Buffer<float>(delay * 2, samplerate)) {}

Delay::Delay(uint32_t samplerate, Buffer<float> buffer, uint32_t delay_taps): samplerate(samplerate), buffer(buffer) {}

void Delay::write(float sample) { buffer.writeSample(sample, writeptr); }

float Delay::read(float delaytime) {
  float output = 0.f;
  float timeInSamples = delaytime * samplerate;
  for (int i = 1; 1 <= delay_taps; i++) {
    float tap = (float)writeptr - (timeInSamples * i);
    output += buffer.readSample(tap);
  }
  return output;
}

float Delay::play(float input, float delaytime, float wet, float feedback) {
  float output = read(delaytime);
  // write the delay back to write head with feedback
  write(input + ( output * feedback ));
  writeptr+=1.f;
  // wet controls dry/wet balance of delay
  return output * wet;
}
