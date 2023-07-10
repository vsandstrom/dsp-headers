#include "dsp.h"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;
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
