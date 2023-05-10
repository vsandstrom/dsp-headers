#include <cmath>
#include "wavetable.hpp"
#include "interpolation.hpp"

#include <memory>

#ifndef WAVETABLE_CPP
#define WAVETABLE_CPP 

using namespace dspheaders;

    WaveTable::WaveTable() {
      position = 0;
      samplerate_ = 48000;
      populateTable(SINE);
      interpolationType = LINEAR;
    };

    WaveTable::WaveTable(int sr) {
      position = 0;
      samplerate_ = sr;
      populateTable(SINE);
      interpolationType = LINEAR;
    };

    WaveTable::WaveTable(
        WAVESHAPE waveshape, int samplerate, INTERPOLATION interpolation)
      : samplerate_(samplerate), interpolationType(interpolation)
    {
      // Requests a +1 memory block to do one less comparison in linear interpolation
      position = 0;
      populateTable(waveshape);

    };


    WaveTable::WaveTable(
        float* wavetable, int samplerate, INTERPOLATION interpolation)
      : samplerate_(samplerate), interpolationType(interpolation) 
    {
      position = 0;
    };

    WaveTable& WaveTable::operator=(const WaveTable& w) {
        memcpy(&table, &w.table, sizeof(float) * w.tableLength);
        frequency = w.frequency;
        position = w.position;
        return *this;
    }

    WaveTable::~WaveTable() {
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


    void WaveTable::movePointer(float phase) {
        // This phase modulation cannot handle negative phase.
        float normalizedPhase = fabs((phase + 1) * 0.5);
        position += tableLength / (samplerate_ / (frequency * normalizedPhase));
        while (position > tableLength) {
          position -= tableLength;
        }
    }


    void WaveTable::movePointer() {
      position += tableLength / (samplerate_ / frequency);
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


    float WaveTable::interpolate() {
      
      switch (interpolationType) {
        case (LINEAR) : {
          return Interpolation::linear(position, table);
        } 

        case (COSINE) : {
          return Interpolation::cosine(position, table);
        }

        // case (CUBIC) : {
        //   // TODO
        //   printf("CUBIC INTERPOLATION NOT IMPLEMENTED");
        //   exit(1);
        //   D(printf("CUBIC\n"));
        //   break;
        // }

        default: {
          return table[(int)position];
        }
      }
} /* namespace dspheaders */

#endif


