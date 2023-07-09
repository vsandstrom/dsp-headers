#include "dsp.h"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;
// A bunch of different constructors
template<typename T>
BaseDelay<T>::BaseDelay(unsigned int samplerate, float delay, unsigned int delay_taps): 
  delay(delay),
  samplerate(samplerate),
  buffer(T(delay * 2, samplerate)) {
  initBuffer();
}

template<typename T>
BaseDelay<T>::BaseDelay(unsigned int samplerate, T buffer, unsigned int delay_taps):
  samplerate(samplerate), 
  buffer(buffer) {
  initBuffer();
}

template<typename T>
void BaseDelay<T>::write(float sample) {
  buffer.writeSample(sample, wrap(writeptr, buffer.bufferLength));
  writeptr++;
}

template<typename T>
float BaseDelay<T>::read(float delaytime) {
  // Non-interpolating read function
  float output = 0.f;
  int tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    int tap = writeptr - (tapdelaytime * i);
    output += buffer.readSample(wrap(tap, buffer.bufferLength));
  }
  return output;
}

template<typename T>
float BaseDelay<T>::play(float input, float delaytime, float wet, float feedback) {
  float output = read(delaytime);
  // write the delay back to write head with feedback
  write(input + ( output * feedback ));
  // wet controls dry/wet balance of delay
  return output * wet;
}

float Delay::read(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    output += buffer.readSample(wrapf(tap, (float)buffer.bufferLength));
  }
  return output;
}

float DelayL::read(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    output += buffer.readSample(wrapf(tap, (float)buffer.bufferLength));
  }
  return output;
}

float DelayC::read(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    output += buffer.readSample(wrapf(tap, (float)buffer.bufferLength));
  }
  return output;
}
