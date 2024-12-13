
#include "../dsp/interpolation.hpp"
#include "../portaudio/include/portaudio.h"
#include <cstdio>
#include <iostream>
#include <sys/_types/_mode_t.h>
#include "../dsp/wavetable.hpp"
#include "../dsp/waveshape.h"



// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           10000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int SIZE =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

using namespace dspheaders;
using namespace interpolation;

Wavetable carrier = Wavetable::init(SAMPLE_RATE);
Wavetable envelope = Wavetable::init(SAMPLE_RATE);

float car_t[SIZE+1] = {0.f};
float amps[] = {1.f, 0.72f, 0.2f, 0.9f};
float phases[] = {0.f, 0.2f, 0.94f, 0.5f};
float env_t[SIZE+1] = {0.f};


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
	frame* data = (frame*) userdata;
	float* out = (float*)outputBuffer;
	unsigned int i;

	(void) inputBuffer; // prevent unused variable warning

	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    float car = carrier.play<SIZE, linear>(car_t, FREQ, 0.f);
    float env = envelope.play<SIZE, linear>(env_t, ENV_FREQ, 0.f);
    // Stereo frame: two increments of out buffer
    *out++ = car * env; 
    *out++ = car * env;
	}
	return 0;
}

int main(int argc, char** argv) {
  complex_sine(car_t, SIZE, amps, phases, 4);
  hanning(env_t, SIZE);
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
