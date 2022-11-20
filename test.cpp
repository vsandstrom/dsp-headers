
#include "portaudio.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include "sound.hpp"


#ifdef DEBUG
    #define D(x) x
#else
    #define D(x)
#endif

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
#define FREQ            300
#define FM_FREQ         180
#define ENV_FREQ         0.250 

// MASTER VOLUME OF THE GENERATED TONE
#define AMP             0.2
// DURATION OF THE GENERATED TONE
#define DURATION		(4000) // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
// #define TABLE_LEN       512
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
#define SAMPLE_RATE		(48000)

// Using TABLE_LEN + 1 to interpolate easier without doing a "out of bounds"-check.
double TABLE[TABLE_LEN + 1] = {0};
double ENV_TABLE[TABLE_LEN + 1] = {0};
double FM_TABLE[TABLE_LEN + 1] = {0};

// initialize structs as global values
wavetable carrier = {TABLE, TABLE_LEN, 0, FREQ, SAMPLE_RATE, interpolate, calcPosition};
wavetable envelope = {ENV_TABLE, TABLE_LEN, 0, ENV_FREQ, SAMPLE_RATE, interpolate, calcPosition};
wavetable modulator = {FM_TABLE, TABLE_LEN, 0, FM_FREQ, SAMPLE_RATE, interpolate, calcPosition};

static frame data;

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(  const void* inputBuffer,						// input
						void* outputBuffer,								// output
						unsigned long framesPerBuffer,					// length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		// :
						PaStreamCallbackFlags statusFlags,
						void* userdata )								// "void"-type can be typecast to other
{

	// cast data passing through stream
	frame* data = (frame*) userdata;
	float* out = (float*) outputBuffer;
	unsigned int i;
	(void) inputBuffer; // prevent unused variable warning
    
    
    // våglängden är samplerate / hertz -> samples per våglängd
    double freq = FREQ;
    double lfo = ENV_FREQ;
    double diff = 0, bWeight = 0, fWeight = 0;
    double samplePerHertz = 0, steps = 0;
    
    // D(for (int i = 0; i < carrier.tablelenght; ++i) {
    //         printf("|%f%|", carrier.table[i]);
    //     }
    //     printf("\n");
    // )
    // D(for (int i = 0; i < envelope.tablelenght; ++i) {
    //         printf("|%f%|", envelope.table[i]);
    //     }
    //     printf("\n");
    // )

	// Write saw waveform to buffer
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer

        // multiply signal by the envelope table
        // WRITE FROM "FAST" TABLE                            WRITE FROM "SLOW" TABLE
        data -> left = carrier.interpolate(
                &carrier, 
                LINEAR
                // PHASE
                )
            * envelope.interpolate(
                &envelope, 
                LINEAR
            );
        data -> right = carrier.interpolate(
                &carrier,
                LINEAR
                // PHASE
            ) 
            * envelope.interpolate(
                &envelope,
                LINEAR 
            );

        // write data to the out buffer
        *out++ = data -> left; // LEFT			-- travel through buffer and fill with samples
		*out++ = data -> right; // RIGHT		-- 'out' variable is dereferenced before filled

        carrier.calcPosition(&carrier, modulator.interpolate(&modulator, LINEAR));
        envelope.calcPosition(&envelope, 1); 
        modulator.calcPosition(&modulator, 1);

        while (carrier.position > carrier.tablelenght) {
            carrier.position -= carrier.tablelenght;
        }
        while (envelope.position > envelope.tablelenght) {
            envelope.position -= envelope.tablelenght;
        }
        while (modulator.position > modulator.tablelenght) {
            modulator.position -= modulator.tablelenght;
        }

	}
	return 0;
}


int main(int argc, char** argv) {
    if ( argc > 3 && argc < 8 ) {
        argc--;
        argv++;
        while (argc > 0){
            if ((*argv)[0] == '-') {
                printf("%c\n", (*argv)[1]);
                switch ((*argv)[1]){
                    case 'c':{
                                argc--;
                                argv++;
                                carrier.frequency = std::stod(*argv);
                                break;
                             }
                    case 'e':{
                                argc--;
                                argv++;
                                envelope.frequency = std::stod(*argv);
                                break;
                             }
                    case 'm':{
                                argc--;
                                argv++;
                                modulator.frequency = std::stod(*argv);
                                break;
                             }
                    default:{

                            argc--;
                            argv++;
                            break;
                            }
                }

            }
            argc--;
            argv++;
        }
        printf("running user input frequencies");
    } else {
        printf("running on default frequencies\n");
    }
    
    populateTable(carrier.table, carrier.tablelenght, SINE);
    populateTable(modulator.table, carrier.tablelenght, ENV);
    populateTable(envelope.table, envelope.tablelenght, ENV);
    // populateTable(carrier, SINE);
    // populateTable(&envelope, ENV);
    D(std::cout << "Populated tables";)

	PaStream* stream;
	PaError err;

    D(std::cout << "PortAudio Test.\n";)

    // initialize first value, no wierd garbage value
    // if they are initialized here, make sure to give the variables the correct values
    // before using it, otherwise there will be an unwanted '0'-sample at the first block
	data.left = data.right = 0.0;

	err = Pa_Initialize();
	if ( err != paNoError ) goto error;
    

	// open an audio I/O stream:
	err = Pa_OpenDefaultStream( &stream,  // < --- Callback is in err
								0, 
								2,
								paFloat32,
								SAMPLE_RATE,
								256, 
								paCallback,
								&data
			);

	if( err != paNoError ) goto error;

	// start sound
	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;

	// sound duration
	Pa_Sleep(DURATION); // NUM_SECONDS is in milliseconds????

	// stop sound
	err = Pa_StopStream(stream);
	if( err != paNoError ) goto error;
	
	err = Pa_CloseStream(stream);
	if( err != paNoError ) goto error;

	Pa_Terminate();
	std::cout << "Test Finished.\n";
	return err;
error:
	Pa_Terminate();

	std::fprintf( stderr, "An error occurred while using the portaudio stream\n" );
	std::fprintf( stderr, "Error number: %d\n", err );
	std::fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ));
	return err;
}



