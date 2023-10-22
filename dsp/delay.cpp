#include "dsp.h"
#include "buffer.hpp"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;

Delay::Delay(
    unsigned samplerate,
    float time,
    unsigned taps
    ) : buffer(Buffer(time, samplerate)),
        samplerate(samplerate), 
        delay_taps(taps) { }

Delay::Delay(
    unsigned samplerate,
    float time
    ) : buffer(Buffer(time, samplerate)), 
        samplerate(samplerate), 
        delay_taps(0) { }

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
  buffer.writesample(sample, (int)writeptr++);
  // wrap_dangerously(&writeptr, buffer.bufferlength);
}

//////////////////////////////////////////////////////////
////////////////// Read for REVERB: //////////////////////
//////////////////////////////////////////////////////////
float Delay::read(int offset) {
  float tap = (float)writeptr + offset;
  return buffer.readsample(tap);
}

void Delay::write(float sample, int offset) {
  // Within bounds-checking is handled in the Buffer object
  buffer.writesample(sample, (int)writeptr + offset);

  // wrap_dangerously(&writeptr, buffer.bufferlength);
}
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

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
