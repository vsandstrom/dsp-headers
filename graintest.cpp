#include <cstdio>
#include <cstdlib>
#include <portaudio.h>
#include <iostream>
#include "dsp/dsp.h"
#include "dsp/interpolation.hpp"
#include "dsp/grain.hpp"
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include "dsp/interpolation.hpp"
#include "dsp/trigger.hpp"

// DURATION OF THE GENERATED TONE
const int DURATION =           30000; // milliseconds
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000;

using namespace dspheaders;

Granulator* gr;
Buffer buf = Buffer(8.f, SAMPLE_RATE, interpolation::linear);
Impulse trigger = Impulse(2.f, SAMPLE_RATE);

int writeptr = 0;

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

  gr->setGrainSize(0.4);
  gr->setJitter(0.4);

    
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    buf.writesample(in[i], writeptr++ % buf.bufferlength);
    // write and increment output and input buffer simultaneously. 
    // hardcoded for a stereo i/o setup
    float gryn = gr->process(*in++, 0.4, 1.f, trigger.play()); 
    *out++ = gryn;
    *out++ = gryn;
	}
	return 0;
}

int main(int argc, char** argv) {

  srand(time(NULL));
  gr = new Granulator(SAMPLE_RATE, 8, &buf, interpolation::linear);
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
