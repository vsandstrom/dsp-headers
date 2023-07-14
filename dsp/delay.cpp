#include "dsp.h"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;
float DelayL::read(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    // Within bounds-checking is handled in the Buffer object
    output += buffer.readSample(tap);
  }
  return output;
}

float DelayL2::read(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    // Within bounds-checking is handled in the Buffer object
    output += buffer.readSample(tap);
  }
  return output;
}

float DelayC::read(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    // Within bounds-checking is handled in the Buffer object
    output += buffer.readSample(tap);
  }
  return output;
}

float DelayH::read(float delaytime) {
  float output = 0.f;
  float tapdelaytime = (delaytime * samplerate);
  for (int i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (tapdelaytime * i);
    // Within bounds-checking is handled in the Buffer object
    output += buffer.readSample(tap);
  }
  return output;
}
