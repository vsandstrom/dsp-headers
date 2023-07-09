#include "dsp/dsp.h"
#include "portaudio.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <string>
#include "dsp/wavetable.hpp"
#include "dsp/vectoroscillator.hpp"
#include "dsp/buffer.hpp"
#include "dsp/delay.hpp"

// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           10000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
constexpr int TABLE_LEN =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

using namespace dspheaders;
WaveTable carrier = WaveTable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, CUBIC);
WaveTable modulator = WaveTable(SINE, TABLE_LEN, SAMPLE_RATE, CUBIC);
WaveTable envelope = WaveTable(ENV, TABLE_LEN, SAMPLE_RATE, CUBIC);
WaveTable delaymod = WaveTable(ENV, TABLE_LEN, SAMPLE_RATE, CUBIC);

Buffer buf = Buffer(4.f, (uint32_t)SAMPLE_RATE);
Delay delay = Delay(SAMPLE_RATE, buf, 2);

static frame data;


// callback function must contain these inputs as PortAudio expects it.
static int paCallback(  const void* inputBuffer,				// input
						void* outputBuffer,								          // output
						unsigned long framesPerBuffer,					    // length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		//
						PaStreamCallbackFlags statusFlags,          //
						void* userdata )								            // "void"-type can be typecast to other 
{

	// cast data passing through stream
	float* out = (float*)outputBuffer;
	unsigned int i;

	(void) inputBuffer; // prevent unused variable warning

    
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    float env = envelope.play();
    float sig = carrier.play(modulator.play()) * env;
    // float del = 0.0f;
    float del = delay.play(sig, map(delaymod.play(), 0.f, 1.f, 1.2f, 3.f), 0.8f, 0.7f);
    
    // write data to the out buffer
    *out++ = sig + del; 
    *out++ = sig + del;
	}
	return 0;
}

int main(int argc, char** argv) {
  carrier.frequency = FREQ;
  modulator.frequency = FM_FREQ;
  envelope.frequency = ENV_FREQ;
  delaymod.frequency = 0.05;
    // if ( argc > 3 && argc < 8 ) {
    //   argc--;
    //   argv++;
    //   while (argc > 0){
    //     if ((*argv)[0] == '-') {
    //       printf("%c\n", (*argv)[1]);
    //       switch ((*argv)[1]){
    //         case 'v': {
    //           argc--;
    //           argv++;
    //           // carrier.frequency = std::stof(*argv);
    //           vec.frequency = std::stof(*argv);
    //           break;
    //         }
    //         case 'e':{
    //           argc--;
    //           argv++;
    //           envelope.frequency = std::stof(*argv);
    //           break;
    //         }
    //         default:{
    //           argc--;
    //           argv++;
    //           break;
    //
    //         }
    //       }
    //     }
    //     argc--;
    //     argv++;
    //   }
    //   printf("running user input frequencies\n");
    // } else {
    //   printf("running on default frequencies\n");
    // }

  // transfer is used as guide wave to determine where between the tables in the vectoroscillator
  // we want to read.
  // This readpointer needs to be positive
  // transfer.frequency = 0.2f;

	PaStream* stream;
	PaError err;

  // initialize first value, no wierd garbage value
  // if they are initialized here, make sure to give the variables the correct values
  // before using it, otherwise there will be an unwanted '0'-sample at the first block
	data.left = data.right = 0.0f;

	err = Pa_Initialize();
	if ( err != paNoError ) goto error;

	// open an audio I/O stream:
	err = Pa_OpenDefaultStream( &stream,  // < --- Callback is in err
								0, 
								2,
								paFloat32,
								(int)SAMPLE_RATE,
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
