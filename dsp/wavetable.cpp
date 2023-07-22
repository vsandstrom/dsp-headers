#include <cmath>
#include "dsp.h"
#include "wavetable.hpp"
#include "interpolation.hpp"

using namespace dspheaders;
WaveTable::WaveTable(
	WAVESHAPE waveshape, unsigned int tableLength, unsigned int samplerate, INTERPOLATION interpolation)
  : tableLength(tableLength), samplerate(samplerate), interpolationType(interpolation)
{
  // Requests a +1 memory block to do one less comparison in linear interpolation
  table = new float[tableLength+1];
  position = 0;
  populateTable(waveshape);

};

WaveTable::WaveTable(unsigned int samplerate): samplerate((float) samplerate){}

WaveTable::WaveTable(
	float* wavetable, unsigned int tableLength, unsigned int samplerate, INTERPOLATION interpolation)
  : tableLength(tableLength), samplerate(samplerate), interpolationType(interpolation) 
{
  table = wavetable;
  position = 0;
};


float WaveTable::play(){
	float out = interpolate();
	movePointer();
	return out;
}

float WaveTable::play(float phase) {
	float out = interpolate();
	movePointer(phase);
	return out;
}
  
void WaveTable::normalize() {
  dspheaders::range(table, tableLength, -1.f, 1.f, 0.f, 1.f);
}

void WaveTable::movePointer(float phase) {
	float normalizedPhase = (phase + 1) * 0.5;
  position += tableLength / (samplerate / (frequency * normalizedPhase));
  position = wrapf(position, tableLength);
}

void WaveTable::movePointer() {
  position += tableLength / (samplerate / frequency);
  position = wrapf(position, tableLength);
}

void WaveTable::populateTable(WAVESHAPE waveshape) {
  float inc = 0, angle = 0, numSamples = (float) tableLength;
  switch (waveshape) {
    case (SINE) : {
      inc = pi * 2.0f  / numSamples;
      for (int i = 0; i < tableLength; ++i) {
        table[i] = sin(angle);
        angle += inc;
      }
      break;
    }

    case (SAW) : {
      inc = 2.0 / numSamples;
      for (int i = 0; i < tableLength; ++i) {
        table[i] =  angle - 1.0;
        angle += inc;
      }
      break;
    }

    case (ENV) : {
      // A hanning window style envelope (squared sine)
      inc = pi / numSamples;
      for (int i = 0; i < tableLength; ++i) {
        table[i] = 1.0 - cos(angle) * cos(angle); 
        angle += inc;
      }
      break;
    }

    case (TRIANGLE) : {
      inc = 2.0 / (numSamples / 2);
      for (int i = 0; i < tableLength; ++i) {
        if ( angle > 1.0 || angle < -1.0) {
          inc *= -1.0;
        }
        table[i] = angle;
        angle += inc;
        }
      break;
    }

    case (SQUARE) : {
      float value = 1.0f;
      for (int i = 0; i < tableLength; ++i) {
        table[i] = value;
        if(i == tableLength/2) {
          value = -1.0f;
        }
      }
      break;
    }

    default:
      break;
  }
}

float WaveTable::interpolate() {
  return Interpolation::linear(wrapf(position, tableLength), table);
}

// float WaveTableL::interpolate() {
//     return Interpolation::linear(wrapf(position, tableLength), table);
// }
//
//
// float WaveTableC::interpolate() {
//     return Interpolation::cubic(wrapf(position, tableLength), table, tableLength);
// }
