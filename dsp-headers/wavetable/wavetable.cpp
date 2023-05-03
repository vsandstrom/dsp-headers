#include <cmath>
#include <memory>
#include "wavetable.hpp"
#include "../interpolation/interpolation.hpp"

#ifndef WAVETABLE_CPP
#define WAVETABLE_CPP 


void WaveTable::init(int samplerate)
{
  // Requests a +1 memory block to do one less comparison in linear interpolation
  position = 0;
  populateTable(WAVESHAPE::TRIANGLE);

};

void WaveTable::init(
	float* wavetable,int samplerate)
{
  memcpy(&table, &wavetable, sizeof(float)*tableLength);
  position = 0;
};

void WaveTable::movePointer(float phase) {
	// This phase modulation cannot handle negative phase.
	float normalizedPhase = fabs((phase + 1) * 0.5);
    position += tableLength / (samplerate / (frequency * normalizedPhase));
    while (position > tableLength) {
      position -= tableLength;
    }
}

void WaveTable::movePointer() {
  position += tableLength / (samplerate / frequency);
  while (position > tableLength) {
    position -= tableLength;
  }
}

void WaveTable::populateTable(WAVESHAPE waveshape) {
  float inc = 0, angle = 0, numSamples = (float) tableLength;

  switch (waveshape) {
    case (SINE) : {
      inc = pi * 2  / numSamples;
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

void WaveTable::setFreq(float f) {
    frequency = f;
}

void WaveTable::setWave(WAVESHAPE ws) {
    waveshape = ws;
}

inline const float WaveTable::getSampleL(const float p) const{
    return 0.f;
}

float WaveTable::getSampleL(float p){
    return 0.f;
}

inline const float WaveTable::getSampleCos(const float p) const{
    return 0.f;
}


#endif

