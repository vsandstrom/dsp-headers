#include <cmath>
#include <cstdio>
#include "wavetable.hpp"

#ifndef WAVETABLE_CPP
#define WAVETABLE_CPP 

#ifdef DEBUG
    #define D(x) x
#else
    #define D(x)
#endif
#ifdef P
  #define PHASE(x) x
#else 
  #define PHASE(x)
#endif

WaveTable::WaveTable(WAVESHAPE waveshape, int tableLength, INTERPOLATION interpolation, int samplerate): tableLength(tableLength), interpolationType(interpolation), samplerate(samplerate) {
  // Requests a +1 memory block to do one less comparison in linear interpolation
  table = (float*)calloc(tableLength + 1, sizeof(float));
  position = 0;
  populateTable(waveshape);
};

WaveTable::WaveTable(float* wavetable, int tableLength, INTERPOLATION interpolation, int samplerate): table(wavetable), tableLength(tableLength), interpolationType(interpolation), samplerate(samplerate) {
  table = wavetable;
  position = 0;
};

WaveTable::~WaveTable() {
  free(table);
}

void WaveTable::movePointer(float phase){
    position += tableLength / (samplerate / (frequency * phase));
    while (position >= tableLength) {
      position -= tableLength;
    }
    D(printf("position: %f", position));
}

void WaveTable::movePointer(){
    position += tableLength / (samplerate / frequency);
    while (position >= tableLength) {
      position -= tableLength;
    }
    D(printf("position: %f", position));
}

void WaveTable::populateTable(WAVESHAPE waveshape){
  float inc = 0, angle = 0, numSamples = (float) tableLength;

  switch (waveshape) {
    case (SINE) : {
      inc = PI * 2  / numSamples;
      for (int i = 0; i < tableLength; ++i) {
        table[i] = sin(angle);
        angle += inc;
      }
      D(printf("SINE\n"));
      break;
    }

    case (SAW) : {
      inc = 2.0 / numSamples;
      for (int i = 0; i < tableLength; ++i) {
        table[i] =  angle - 1.0;
        angle += inc;
      }
      D(printf("SAW\n"));
      break;
    }

    case (ENV) : {
      // A hanning window style envelope (squared sine)
      inc = PI / numSamples;
      for (int i = 0; i < tableLength; ++i) {
        table[i] = 1.0 - cos(angle) * cos(angle); 
        angle += inc;
      }
      D(printf("ENV: 1 - cos(x)^2\n"));
      break;
    }

    case (TRIANGLE) : {
      inc = 4.0 / (numSamples / 4);
      for (int i = 0; i < tableLength; ++i) {
        if ( angle >= 1.0 || angle < -1.0) {
          inc *= -1.0;
        }
        table[i] = angle;
        angle += inc;
        }
      D(printf("TRIANGLE\n"));
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
      D(printf("SQUARE\n"));
      break;
    }

    default:
      break;
  }
}

float WaveTable::interpolate() {
  float nextWeight, prevWeight;
  int prevPosition, nextPosition;
  
  switch (interpolationType) {
    case (LINEAR) : {
      prevPosition = position; // implicit cast
      nextPosition = prevPosition + 1;
      float diff = position - (float)prevPosition;
      nextWeight = diff;
      prevWeight = 1 - diff;
      D(printf("LINEAR\n"));
      break;
    } 

    case (COSINE) : {
      float diff = position - floor(position);
      nextWeight = (1 - cos(diff*PI)) / 2;
      prevWeight = 1.0 - nextWeight;
      D(printf("COSINE\n"));
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
#endif
