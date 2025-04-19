#include <cstdio>
#include <cstring>
#include <iostream>
#include "../portaudio/include/portaudio.h"
#include "../dsp/dsp.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/vectortable.hpp"

// MASTER VOLUME OF THE GENERATED TONE
const float AMP = 0.2f;
// DURATION OF THE GENERATED TONE
const int DURATION = 60000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int SIZE = 512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE = 48000;

const int WIDTH = 3;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

using namespace dspheaders;

VectorOscillatorLinear osc = VectorOscillatorLinear::init(SAMPLE_RATE);
Wavetable lfo = Wavetable::init(SAMPLE_RATE);
float t1[SIZE+1] = {0.f};
float t2[SIZE+1] = {0.f};
float t3[SIZE+1] = {0.f};
float tlfo[SIZE+1] = {0.f};
float* tx[WIDTH] = {t1, t2, t3};

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
    float sig = osc.play<SIZE, WIDTH>((float**)tx, FREQ, lfo.play<SIZE, interpolation::linear>(tlfo, 1.f / (DURATION / 1000.f), 0.0), 0.f) * 0.1;

    // Stereo frame: two increments of out buffer
    *out++ = sig*AMP; 
    *out++ = sig*AMP;
	}
	return 0;
}

int main(int argc, char** argv) {

  // initialize tables
  for (auto& t: tx) {
    for (int i = 0; i < SIZE+1; i++) {
      t[i] = 0.f;
    }
  }

  sine(t1, SIZE);
  float a1[3] = {1.0, 0.2, 0.3};
  float p1[3] = {0.0, 0.25, 0.0};
  complex_sine(t2, SIZE, a1, p1, 3);
  float a2[5] = {1.0, 0.2, 0.3, 0.5, 0.7};
  float p2[5] = {0.0, 0.25, 0.0, 0.75, 0.0};
  complex_sine(t3, SIZE, a2, p2, 5);

  phasor(tlfo, SIZE);

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
