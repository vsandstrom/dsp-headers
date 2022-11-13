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

int bPos = 0;
int fPos = 0;

double interpolate(double pos, double* table) {
    // iterpolate
    double diff = floor(pos);
    double bWeight = pos - diff;
    double fWeight = 1.0 - bWeight;
    bPos = (int)(pos-1.f);
    fPos = (int)pos;
    if (bPos < 0) {
        bPos = 511;
    }

    return (table[bPos] * fWeight) + (table[fPos] * bWeight);
}

void populateTable(double* table, int n, int tabletype ) {
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

    }
}

#endif
