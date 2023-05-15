#include "dsp.h"
#include "delay.hpp"
#include "interpolation.hpp"


#define LIMIT 16.f
// Hard limit for length of buffer

using namespace dspheaders;

Delay::Delay(uint32_t samplerate, float seconds) {
  // Initialize a buffer with the length of N seconds
  if (seconds < LIMIT && seconds > 0) {
    // Prevent too long buffers or buffers with negative length
    bufferLength = samplerate * seconds;
  } else {
    bufferLength = samplerate * LIMIT;
  }
  buffer = new float[bufferLength];
}

void Delay::write(float sample) {
  // Writes input signal to next sample in buffer
  buffer[writeptr] = sample;
  writeptr++;
  while (writeptr > bufferLength) {
    writeptr -= bufferLength;
  }
}

float Delay::read(float speed) {
  // if regular speed == 1 : 1, double speed == 2 samples : 1 sample
  readptr += speed;
  while (readptr > bufferLength) {
    readptr -= bufferLength;
  }
  return Interpolation::linear(readptr, buffer);
}

float Delay::play(float sample, float speed) {
  write(sample);
  return read(speed);
}

