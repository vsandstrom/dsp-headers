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

void populateTable2(wavetable* table, WAVETYPES tabletype ) {
    double inc = 0, angle = 0;
    int i = 0;

    switch (tabletype) {
        case (SINE) : {
            inc = PI * 2  / table -> tablelenght;

            for (i = 0; i < table -> tablelenght; ++i) {
                *table -> table++ = sin(angle);
                angle += inc;
            }
            D(printf("SINE\n"));
            D(for (int i = 0; i < table -> tablelenght; ++i) {
                    printf("|%f%|", table -> table[i]);
                }
                printf("\n");
            )

            break;
        }

        case (SAW) : {
            inc = 2.0 / (double) table->tablelenght;
            for (i = 0; i < table->tablelenght; ++i) {
                table -> table[i] =  angle - 1.0;
                angle += inc;
            }
            D(printf("SAW\n"));
            D(for (int i = 0; i < table -> tablelenght; ++i) {
                    printf("|%f%|", table -> table[i]);
                }
                printf("\n");
            )
            break;
        }

        case (ENV) : {
            // A hanning window style envelope (squared sine)
            inc = PI / (float) table->tablelenght;
            for (i = 0; i < table->tablelenght; ++i) {
                *table -> table++ = 1.0 - cos(angle) * cos(angle); 
                angle += inc;
            }
            D(printf("ENV: 1 - cos(x)^2\n"));
            D(for (int i = 0; i < table -> tablelenght; ++i) {
                    printf("|%f%|", table -> table[i]);
                }
                printf("\n");
            )
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

void populateTable(double* table, int n, WAVETYPES tabletype) {
    double inc = 0, angle = 0, numSamples = (double) n;
    int i = 0;

    switch (tabletype) {
        case (SINE) : {
            inc = PI * 2  / numSamples;

            for (i = 0; i < n; ++i) {
                *table++ = sin(angle);
                angle += inc;
            }
            D(printf("SINE\n"));
            break;
        }

        case (SAW) : {
            inc = 2.0 / numSamples;
            for (i = 0; i < n; ++i) {
                *table++ =  angle - 1.0;
                angle += inc;
            }
            D(printf("SAW\n"));
            break;
        }

        case (ENV) : {
            // A hanning window style envelope (squared sine)
            inc = PI / numSamples;
            for (i = 0; i < n; ++i) {
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
            //
            // TODO
            //
            break;
        }

        default:
            break;

    }
}

double interpolate(wavetable* table, IPTYPE type) {
    // iterpolate
    double currWeight, prevWeight;
    double diff = floor(table -> position);
    // since the table is TABLE_LEN + 1, we can do this and not worry about going out of bounds.
    int prevPos = (int)(table -> position);
    int currPos = (int)table -> position + 1.f;
    

    switch (type) {
        case (LINEAR) : {
            currWeight = table -> position - diff;
            prevWeight = 1.0 - currWeight;
            break;
        } 

        case (COSINE) : {
            currWeight = (1 - cos(diff*PI)) / 2;
            prevWeight = 1.0 - currWeight;
            break;
        }

        case (CUBIC) : {
            // TODO
            printf("CUBIC INTERPOLATION NOT IMPLEMENTED");
            exit(1);
            break;
        }

        case (HERMITE) : {
            printf("CUBIC INTERPOLATION NOT IMPLEMENTED");
            exit(1);
            break;
        }
        default: {
            break;
        }
    }

    

    // D(printf("inside interpolate \n"));

    return (table->table[prevPos] * prevWeight) + (table->table[currPos] * currWeight);
}

void calcPosition(wavetable* table, double phase) {
    table -> position += table -> tablelenght / ( table -> samplerate / (table -> frequency * phase) );
    while (table -> position > table -> tablelenght) table -> position -= table -> tablelenght;
}

// double calcPosition2(wavetable* table) {
//     // calculates how many samples in the table corresponds to the frequency, 
//     // if you want to play an 'A' at 440:
//     // tablelenght: 512 / samplerate: 48000 / frequency: 440
//     return table -> tablelenght / ( table -> samplerate / table -> frequency); // <-- apparently the parenthesis matters
//     
// };

// double calcPosition3(double samplerate, double frequency, double tablelenght) {
//     // calculates how many samples in the table corresponds to the frequency, 
//     // if you want to play an 'A' at 440:
//     // tablelenght: 512 / samplerate: 48000 / frequency: 440
//     return tablelenght / (samplerate / frequency); // <-- apparently the parenthesis matters
// };



// double interpolate2(double pos, double* table) {
//     // iterpolate
//     int prevPos, currPos;
//     double diff = floor(pos);
//     double currWeight = pos - diff;
//     double prevWeight = 1.0 - currWeight;
//     prevPos = (int)(pos-1.f);
//     currPos = (int)pos;
//     if (prevPos < 0) {
//         prevPos = 511;
//     }
//
//     return (table[prevPos] * prevWeight) + (table[currPos] * currWeight);
// }
#endif


