#include <cmath>
#include "wavetable.hpp"

#ifndef WAVETABLE_CPP
#define WAVETABLE_CPP 

WaveTable::WaveTable(
	WAVESHAPE waveshape, int tableLength, int samplerate, INTERPOLATION interpolation)
  : tableLength(tableLength), samplerate(samplerate), interpolationType(interpolation)
{
  // Requests a +1 memory block to do one less comparison in linear interpolation
  table = new float[tableLength];
  position = 0;
  populateTable(waveshape);

};

WaveTable::WaveTable(
	float* wavetable, int tableLength, int samplerate, INTERPOLATION interpolation)
  : tableLength(tableLength), samplerate(samplerate), interpolationType(interpolation) 
{
  table = wavetable;
  position = 0;
};


WaveTable::~WaveTable() {
  delete [] table;
}



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
      inc = PI * 2  / numSamples;
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
      inc = PI / numSamples;
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
  float nextWeight = 0.0f, prevWeight = 0.0f;
  int prevPosition = 0, nextPosition = 0;
  
  switch (interpolationType) {
    case (LINEAR) : {
      prevPosition = position; // implicit cast
      nextPosition = prevPosition + 1;
      float diff = position - (float)prevPosition;
      nextWeight = diff;
      prevWeight = 1 - diff;
      break;
    } 

    case (COSINE) : {
      float diff = position - floor(position);
      nextWeight = (1 - cos(diff*PI)) / 2;
      prevWeight = 1.0 - nextWeight;
      break;
    }

    // case (CUBIC) : {
    //   // TODO
    //   printf("CUBIC INTERPOLATION NOT IMPLEMENTED");
    //   exit(1);
    //   D(printf("CUBIC\n"));
    //   break;
    // }

    default: {
        break;
    }
  }
  


  return table[prevPosition] * prevWeight + table[nextPosition] * nextWeight;
}

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
#endif

