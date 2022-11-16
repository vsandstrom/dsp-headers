#ifdef DEBUG
    #define D(x) x
#else
    #define D(x)
#endif

#include "portaudio.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "sound.hpp"


// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
#define FREQ            300
#define LFOFREQ         0.250 

// MASTER VOLUME OF THE GENERATED TONE
#define AMP             0.2
// DURATION OF THE GENERATED TONE
#define DURATION		(4000) // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
// #define TABLE_LEN       512
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
#define SAMPLE_RATE		(48000)


double TABLE[TABLE_LEN] = {0};
double ENV_TABLE[TABLE_LEN] = {0};

// initialize structs as global values
wavetable sig {TABLE, TABLE_LEN, 0, FREQ, SAMPLE_RATE, interpolate, calcPosition};
wavetable env {ENV_TABLE, TABLE_LEN, 0, LFOFREQ, SAMPLE_RATE, interpolate, calcPosition};
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
    double lfo = LFOFREQ;
    double diff = 0, bWeight = 0, fWeight = 0;
    double samplePerHertz = 0, steps = 0;
    
    // D(for (int i = 0; i < sig.tablelenght; ++i) {
    //         printf("|%f%|", sig.table[i]);
    //     }
    //     printf("\n");
    // )
    // D(for (int i = 0; i < env.tablelenght; ++i) {
    //         printf("|%f%|", env.table[i]);
    //     }
    //     printf("\n");
    // )

	// Write saw waveform to buffer
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer

        // multiply signal by the envelope table
        // WRITE FROM "FAST" TABLE                            WRITE FROM "SLOW" TABLE
        data -> left = sig.interpolate(&sig) * env.interpolate(&env);
        data -> right = sig.interpolate(&sig) * env.interpolate(&env);
        // data -> left = interpolate(sig.position, sig.table) * interpolate(env.position, env.table);
        // data -> right = interpolate(sig.position, sig.table) * interpolate(env.position, env.table);
        // D(printf("after interpolate\n"));

        // write data to the out buffer
        *out++ = data -> left; // LEFT			-- travel through buffer and fill with samples
		*out++ = data -> right; // RIGHT		-- 'out' variable is dereferenced before filled

        // increment both wavetable and envelope
        // sig.position += calcPosition(SAMPLE_RATE, sig.frequency, sig.tablelenght);
        // env.position += calcPosition(SAMPLE_RATE, env.frequency, env.tablelenght); 
        // sig.position += sig.calcPosition(&sig);
        // env.position += env.calcPosition(&env); 
        sig.calcPosition(&sig);
        env.calcPosition(&env); 
        // D(printf("after calc\n"));

        // use 'while' instead of 'if', to handle if pos > (TABLE_LEN * 2)
        while (sig.position > sig.tablelenght) {
            sig.position -= sig.tablelenght;
        }
        while (env.position > env.tablelenght) {
            env.position -= env.tablelenght;
        }

	}
	return 0;
}


int main(void) {

    populateTable(sig.table, sig.tablelenght, SINE);
    populateTable(env.table, env.tablelenght, ENV);
    // populateTable(sig, SINE);
    // populateTable(&env, ENV);
    D(std::cout << "Populated tables");

	PaStream* stream;
	PaError err;

    D(std::cout << "PortAudio Test.\n";);

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



