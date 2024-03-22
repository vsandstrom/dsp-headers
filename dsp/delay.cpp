
/* ˚◊˚ */

#include "buffer.hpp"
#include "delay.hpp"
#include "interpolation.hpp"

using namespace dspheaders;

Delay::Delay(
    unsigned samplerate,
    float maxdelaytime,
    unsigned taps,
    float (*interpolate)(float, float*, unsigned)
    ) : buffer(Buffer(maxdelaytime, samplerate, interpolate)),
        samplerate(samplerate), 
        delay_taps(taps) { }

Delay::Delay(
    unsigned samplerate,
    float maxdelaytime,
    float (*interpolate)(float, float*, unsigned)
    ) : buffer(Buffer(maxdelaytime, samplerate, interpolate)), 
        samplerate(samplerate), 
        delay_taps(0) { }

float Delay::read(float delaytime, float damp) {
  float output = 0.f;
  float taptime = (delaytime * samplerate);
  for (unsigned i = 1; i <= delay_taps; i++) {
    float tap = (float)writeptr - (taptime*i);
    output += buffer.readsample(tap) * damp;
    // damp*=damp;
    damp /= 2;
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
  int write = writeptr + offset;
  buffer.writesample(sample, write);
  writeptr++;
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

float Delay::process(float input, float delaytime, float wet, float feedback) {
  float output = read(delaytime, feedback);
  // write the time back to write head with feedback
  // MAGIC NUMBER for scaling the feedback of the delay to something managable
  output = interpolation::slope((input + output),prev);
  write(output);
  prev = output;
  // wet controls dry/wet balance of time
  return output * wet;
}

