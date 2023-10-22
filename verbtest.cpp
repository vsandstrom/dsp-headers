
#include <cstdio>
#include <portaudio.h>
#include "dsp/interpolation.hpp"
#include "dsp/filter.hpp"
#include "dsp/verb.hpp"
#include "dsp/wavetable.hpp"
#include <iostream>

// DURATION OF THE GENERATED TONE
const int DURATION =           4000; // milliseconds
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000.f;

using namespace dspheaders;

ChownVerb verb = ChownVerb(SAMPLE_RATE);
static frame data;
bool impulse = true;

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(  const void* inputBuffer,				// input
						void* outputBuffer,								          // output
						unsigned long framesPerBuffer,					    // length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		//
						PaStreamCallbackFlags statusFlags,          //
						void* userdata )								            // "void"-type can be typecast to other 
{
	// cast data passing through stream
	frame* data = (frame*) userdata;
	float* out = (float*)outputBuffer;
	unsigned int i;

  float env = 0.f;
  float venv = 0.f;

  // float verb = 0.f;

	(void) inputBuffer; // prevent unused variable warning


	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    float sig = (impulse == true) ? 1.f : 0.f;
    sig = verb.process(sig, 0.97);

    // Stereo frame: two increments of out buffer
    *out++ = sig*0.3; 
    *out++ = sig*0.3;
    
#ifdef DEBUG
    printf("output: %f\n", sig);
#endif

    impulse = false;
	}
	return 0;
}

int main(int argc, char** argv) {
	PaStream* stream;
	PaError err;
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
