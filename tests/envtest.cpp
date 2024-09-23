#include <cstdio>
#include <iostream>
#include "../portaudio/include/portaudio.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/envelope.hpp"
#include "../dsp/delay.hpp"

// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           10000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int SIZE =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000.f;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

float breakpoints[] = {0.f, 0.2f, 0.8f, 0.f};
float breaktimes[] = {0.1f, 1.5f, 1.5f};
float breakcurves[] = {1.5f, 0.2,  0.2};

// keeps track of the number of the current sample
unsigned timeline = 0;
// Progresses the score
unsigned scoreptr = 0;

using namespace dspheaders;
using namespace interpolation;
Envelope envelope = Envelope(breakpoints, 4, breaktimes, 3, breakcurves, 3, SAMPLE_RATE, interpolation::linear);
Wavetable carrier = Wavetable::init(SAMPLE_RATE);
Wavetable modulator = Wavetable::init(SAMPLE_RATE);
float car_t[SIZE] = {0.f};
float mod_t[SIZE] = {0.f};
Delay delay = Delay(SAMPLE_RATE, 4.f, 4, interpolation::cubic);

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

  float env = 0.f;

	(void) inputBuffer; // prevent unused variable warning

	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer

    if ( timeline == 48000 || timeline == 48000 * 6) {
      env = envelope.play(GATE::on);
    } else {
      env = envelope.play(GATE::off);
    }
    float car = carrier.play<SIZE, cubic>(
      car_t, FREQ, modulator.play<SIZE, cubic>(
        mod_t, FM_FREQ, 0.f)
    );
    float sig = car*env;

    // Stereo frame: two increments of out buffer
    *out++ = sig; 
    *out++ = sig;

    timeline++;
	}
	return 0;
}

int main(int argc, char** argv) {
	PaStream* stream;
	PaError err;

	data.left = data.right = 0.0f;
  triangle(car_t, SIZE);
  sine(mod_t, SIZE);

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
