
#include <cstdio>
#include "../portaudio/include/portaudio.h"
#include "../dsp/dsp.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/envelope.hpp"


// SETUP
const int INPUT_CH = 2;
const int OUTPUT_CH = 2;
const int PROGRAM_DURATION = 120000; // milliseconds
const float  SAMPLE_RATE =   48000;
const int DURLEN = 7;
const unsigned envlength = 512;

using namespace dspheaders;

// GLOBALS
Envelope* env;
float envptr = 0.f;

int duridx = 0;
float durs[DURLEN] = {0.2, 1.2, 0.987, 0.333, 4, 0.01f, 0.05f};

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
    if (envptr >= env->getBufferlength()) {
      envptr = 0.f;
      duridx++;
    }
    if (duridx == DURLEN) {
      sig = 0.f;
    } else {
      envptr += setDur(durs[duridx], envlength, SAMPLE_RATE);
      sig = env->read(envptr);
    }
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

  float table[envlength] = {0.f};
  hanning(table, envlength);

  env = new Envelope(table, envlength, SAMPLE_RATE, interpolation::cubic);


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
