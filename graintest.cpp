#include <cstdio>
#include <cstdlib>
#include <portaudio.h>
#include <iostream>
#include "dsp/dsp.h"
#include "dsp/interpolation.hpp"
#include "dsp/wavetable.hpp"
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
// Impulse trigger = Impulse(0.15f, SAMPLE_RATE);
Dust trigger = Dust(0.15f, SAMPLE_RATE);
Wavetable saw = Wavetable(SAW, 1024, SAMPLE_RATE, interpolation::linear);

int playhead = 0;
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
	float* in = (float*)inputBuffer;
	unsigned int i;
  saw.frequency = 1.f/24;

    
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    // monobuffer -- write once, then grain
    if (writeptr < buf.bufferlength) {
      float input = *in++;
      buf.writesample(input, writeptr % buf.bufferlength);
      writeptr++;
      in++;
      *out++ = 0.f;
      *out++ = 0.f;
    } else {
        float trigg = trigger.play();
        float phasor = map(saw.play(), -1.f, 1.f, 0.f, 0.99f);
        float gryn = gr->process(phasor, 1.f, trigg); 
        *out++ = gryn;
        *out++ = gryn;
    }
    playhead++;
	}
	return 0;
}

int main(int argc, char** argv) {

  srand(time(NULL));
  gr = new Granulator(SAMPLE_RATE, 16, &buf, interpolation::linear);
  gr->setGrainSize(0.2f);
  gr->setJitter(0.05f);
  
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
