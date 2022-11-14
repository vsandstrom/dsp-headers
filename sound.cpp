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


double interpolate(double pos, double* table) {
    // iterpolate
    int prevPos, currPos;
    double diff = floor(pos);
    double currWeight = pos - diff;
    double prevWeight = 1.0 - currWeight;
    prevPos = (int)(pos-1.f);
    currPos = (int)pos;
    if (prevPos < 0) {
        prevPos = 511;
    }

    return (table[prevPos] * prevWeight) + (table[currPos] * currWeight);
}


double calcPosition(double samplerate, double frequency, double tablelenght) {
    // calculates how many samples in the table corresponds to the frequency, 
    // if you want to play an 'A' at 440:
    // tablelenght: 512 / samplerate: 48000 / frequency: 440
    return tablelenght / (samplerate / frequency); // <-- apparently the parenthesis matters
};


void populateTable(double* table, int n, WAVETYPES tabletype ) {
    double inc = 0, angle = 0;
    int i = 0;

    switch (tabletype) {
        case (SINE) : {
            inc = PI * 2  / (float) n;

            for (i = 0; i < n; ++i) {
                *table++ = sin(angle);
                angle += inc;
            }
            D(printf("SINE\n"));
            break;
        }

        case (SAW) : {
            inc = 2.0 / (double) n;
            for (i = 0; i < n; ++i) {
                *table++ =  angle - 1.0;
                angle += inc;
            }
            D(printf("SAW\n"));
            break;
        }

        case (ENV) : {
            // A hanning window style envelope (squared sine)
            inc = PI / (float) n;
            for (i = 0; i < n; ++i) {
                *table++ = 1.0 - cos(angle) * cos(angle); 
                angle += inc;
            }
            D(printf("ENV: 1 - cos(x)^2\n"));
            break;
        }

        case (TRIANGLE) : {
            //
            // TODO
            //
            break;
        }

        case (SQUARE) : {
            //
            // TODO
            //
            break;
        }

        default:
            break;

    }
}

#endif


