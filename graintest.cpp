#include <portaudio.h>
#include <iostream>
#include "dsp/dsp.h"
#include "dsp/interpolation.hpp"
#include "dsp/wavetable.hpp"
#include "dsp/envelope.hpp"
#include "dsp/grain.hpp"
#include "dsp/delay.hpp"

#include <iostream>
#include "dsp/interpolation.hpp"

// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           30000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
constexpr int TABLE_LEN =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000;

static float delaytime = 0.0f;
static float fb = 0.0f;

using namespace dspheaders;

Granulator gr = Granulator(SAMPLE_RATE, 8, interpolation::linear);

static frame data;

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(
            const void* inputBuffer,             				// input
						void* outputBuffer,								          // output
						unsigned long framesPerBuffer,					    // length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		//
						PaStreamCallbackFlags statusFlags,          //
						void* userdata )								            // "void"-type can be typecast to other 
{

	// cast data passing through stream
	float* out = (float*)outputBuffer;
	unsigned int i;

	float* in = (float*)inputBuffer;

  // gr.setJitter(0.1);

    
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    // write and increment output and input buffer simultaneously. 
    // hardcoded for a stereo i/o setup
    *out++ = gr.process(*in++, 0.1, 1.f); 
    *out++ = gr.process(*in++, 0.1, 1.f); 
	}
	return 0;
}

int main(int argc, char** argv) {
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
								2, 
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
