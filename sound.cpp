#include <cmath>
#include <cstdio>
#include "sound.hpp"

#ifndef SOUND
#define SOUND

#ifdef DEBUG
    #define D(x) x
#else
    #define D(x)
#endif



void populateTable(double* table, int n, WAVESHAPE tabletype) {
    double inc = 0, angle = 0, numSamples = (double) n;
    int i = 0;

    switch (tabletype) {
        case (SINE) : {
            inc = PI * 2  / numSamples;

            for (; i < n; ++i) {
                *table++ = sin(angle);
                angle += inc;
            }
            D(printf("SINE\n"));
            break;
        }

        case (SAW) : {
            inc = 2.0 / numSamples;
            for (; i < n; ++i) {
                *table++ =  angle - 1.0;
                angle += inc;
            }
            D(printf("SAW\n"));
            break;
        }

        case (ENV) : {
            // A hanning window style envelope (squared sine)
            inc = PI / numSamples;
            for (; i < n; ++i) {
                *table++ = 1.0 - cos(angle) * cos(angle); 
                angle += inc;
            }
            D(printf("ENV: 1 - cos(x)^2\n"));
            break;
        }

        case (TRIANGLE) : {
            inc = 4.0 / (numSamples / 4);
            for (; i < n; ++i) {
                if ( angle >= 1.0 || angle < -1.0) {
                    inc *= -1.0;
                }
                *table++ = angle;
                angle += inc;
            }
            break;
        }

        case (SQUARE) : {
          double value = 1.0f;
          for (; i < n; ++i) {
            *table++ = value;
            if (i == n/2) {
              value = -1.0f;
            }
          }
            break;
        }

        default:
            break;

    }
}

double interpolate(wavetable* table, INTERPOLATION type) {
    // iterpolate
    double nextWeight, prevWeight;
    double diff = floor(table -> position);
    // since the table is TABLE_LEN + 1, we can do this and not worry about going out of bounds.
    int prevPosition = (int)(table -> position);
    int nextPosition = (int)table -> position + 1;
    
    switch (type) {
        case (LINEAR) : {
            nextWeight = table -> position - diff;
            prevWeight = 1.0 - nextWeight;
            break;
        } 

        case (COSINE) : {
            nextWeight = (1 - cos(diff*PI)) / 2;
            prevWeight = 1.0 - nextWeight;
            break;
        }

        case (CUBIC) : {
            // TODO
            printf("CUBIC INTERPOLATION NOT IMPLEMENTED");
            exit(1);
            break;
        }

        default: {
            break;
        }
    }
    return (table->table[prevPosition] * prevWeight) + (table->table[nextPosition] * nextWeight);
}


void calcPosition(wavetable* table, double phase) {
  // Determines which sample to read next, taking into account the oscillators frequency and the
  // global samplerate, then updates the WaveTable-structs *position* member. This also allows for a
  // *phase* input, easily translated to frequency modulation, or *phase modulation*.
    table -> position += table -> tableLength / ( table -> samplerate / (table -> frequency * phase) );
    while (table -> position > table -> tableLength) table -> position -= table -> tableLength;
}

void calcPosition(wavetable *table) {
  // The same function as above, but omits the phase argument
    table -> position += table -> tableLength / ( table -> samplerate / (table -> frequency) );
    while (table -> position > table -> tableLength) table -> position -= table -> tableLength;

}

#endif


