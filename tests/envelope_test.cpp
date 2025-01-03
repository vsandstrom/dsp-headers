
#include <cstdio>
#include <vector>
#include "../portaudio/include/portaudio.h"
#include "../dsp/dsp.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/envelope.hpp"
#include "../dsp/wavetable.hpp"


// SETUP
const int INPUT_CH = 2;
const int OUTPUT_CH = 2;
const int PROGRAM_DURATION = 120000; // milliseconds
const float  SAMPLE_RATE =   48000;
const int DURLEN = 7;
const unsigned envlength = 512;

using namespace dspheaders;

// GLOBALS
std::vector<BreakPoint> bkp {
  {0.f, 0.0, 0.0},
  {.8f, 0.2, 0.0},
  {0.f, 1.2, 0.0},
};

Envelope env = Envelope::init(bkp, SAMPLE_RATE);
float envptr = 0.f;

int duridx = 0;
float durs[DURLEN] = {0.2, 1.2, 0.987, 0.333, 4, 0.01f, 0.05f};

Wavetable wt = Wavetable(TRIANGLE, 512, SAMPLE_RATE, interpolation::linear);


static frame data;

float setDur(float dur, unsigned length, float samplerate);

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(
            const void* inputBuffer,             				// input
						void* outputBuffer,								          // output
						unsigned long framesPerBuffer,					    // length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		//
						PaStreamCallbackFlags statusFlags,          //
						void* userdata)								              //
{
	// cast data passing through stream
	float* out = (float*)outputBuffer;
	float* in = (float*)inputBuffer;
  int i;
  float sig = 0.f;
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    sig = env.play();
    *out++ = sig;
    *out++ = sig;
	}
	return 0;
}

float setDur(float dur, unsigned length, float samplerate) {
  // 512 (len) / 48000 (sr) * 0.2 (sec) ≈ 0.05333334 samples/frame
  return length / (samplerate * dur); 
};

float setFloatDur(float dur, unsigned length, float samplerate) {
  // 512 (len) / 48000 (sr) * 0.2 (sec) ≈ 0.05333334 samples/frame
  return ((float)length / samplerate) * dur; 
};

int main(int argc, char** argv) {
  env.set_loopable(true);

	PaStream* stream;
	PaError err;
  // Initialize silence
	data.left = data.right = 0.0f;
	err = Pa_Initialize();
	if ( err != paNoError ) goto error;
	// open an audio I/O stream:
	err = Pa_OpenDefaultStream( &stream,
                              INPUT_CH, 
                              OUTPUT_CH, 
                              paFloat32,
                              (int)SAMPLE_RATE,
                              256,
                              paCallback,
                              &data );

	if( err != paNoError ) goto error;
	// start sound
	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;
	// sound duration
	Pa_Sleep(PROGRAM_DURATION);
	// stop sound
	err = Pa_StopStream(stream);
	if( err != paNoError ) goto error;
	err = Pa_CloseStream(stream);
	if( err != paNoError ) goto error;
	Pa_Terminate();
	printf("Test Finished.\n");
	return err;
error:
	Pa_Terminate();
	std::fprintf( stderr, "An error occurred while using the portaudio stream\n" );
	std::fprintf( stderr, "Error number: %d\n", err );
	std::fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ));
	return err;
}
