#include "../dsp/dsp.h"
#include "../dsp/interpolation.hpp"
#include "../portaudio/include/portaudio.h"
#include <cstdio>
#include <iostream>
#include "../dsp/filter.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/trigger.hpp"

// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           10000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int TABLE_LEN =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

using namespace dspheaders;

Wavetable *carrier = nullptr;

bool impulse = true;

Comb c0 = Comb::init<17>(0.7, 0.7);
Comb c1 = Comb::init<23>(0.7, 0.7);
Comb c2 = Comb::init<27>(0.7, 0.7);
Comb c3 = Comb::init<41>(0.7, 0.7);
Wavetable lfo = Wavetable::init(SAMPLE_RATE);
Dust imp = Dust::init(SAMPLE_RATE);

float lfo_t[TABLE_LEN] = {0};



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
  float o0 = 0.f;
  float o1 = 0.f;
  float o2 = 0.f;
  float o3 = 0.f;
  float l  = 0.f;

	(void) inputBuffer; // prevent unused variable warning

	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    float sig = imp.play(0.2);
      //(impulse == true) ? 1.f : 0.f;
    l = lfo.play<TABLE_LEN, interpolation::linear>(lfo_t, 5.4, 0.f);
    o0 = c0.process(sig);
    o1 = c1.process(sig);
    o2 = c2.process(sig);
    o3 = c3.process(sig);

    sig += (o0 + o1 + o2 + o3) * 0.2;

    // Stereo frame: two increments of out buffer
    *out++ = sig; 
    *out++ = sig;
    impulse = false;
	}
	return 0;
}

int main(int argc, char** argv) {
	PaStream* stream;
	PaError err;

	data.left = data.right = 0.0f;

  triangle(lfo_t, TABLE_LEN);

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
