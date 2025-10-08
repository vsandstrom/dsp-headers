#include <cstddef>
#include <cstdio>
#include "../portaudio/include/portaudio.h"
#include "../dsp/filter.hpp"
#include "../dsp/filterbank.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/interpolation.hpp"
#include "../dsp/waveshape.h"


// SETUP
constexpr int INPUT_CH = 2;
constexpr int OUTPUT_CH = 2;

constexpr int PROGRAM_DURATION = 120000; // milliseconds
constexpr float  SAMPLE_RATE =   48000;

using namespace dspheaders;

// GLOBALS
FilterBank8<5> bank;
Osc z1 = Osc<interpolation::none>::init(SAMPLE_RATE);
std::vector<Osc<interpolation::none>> va = {
  Osc<interpolation::none>::init(SAMPLE_RATE),
  Osc<interpolation::none>::init(SAMPLE_RATE),
  Osc<interpolation::none>::init(SAMPLE_RATE),
  Osc<interpolation::none>::init(SAMPLE_RATE),
  Osc<interpolation::none>::init(SAMPLE_RATE)
};

float va_freq[5] = { 0.2f, 0.32f, 0.54f, 0.89f, 1.11f, };

float saw_table[512];
float hanning_table[512];

static frame data;

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
	unsigned i = 0;

	for (; i < framesPerBuffer; i++) { // loop over buffer
    float sig = z1.play(saw_table, 512, 127.f, 0.f);
    for (size_t i = 0; i < bank.size(); i++) {
      bank.setAmp(i, va[i].play(hanning_table, 512, va_freq[i], 0.f));
    }
    float filt = bank.process(sig);
    *out++ = sig;
    *out++ = filt;
    // *out++ = (s1 + s2) / 2;
    // *out++ = (s3 + s4) / 2;
	}
	return 0;
}

int main(int argc, char** argv) {
  float init_freq = 127.f;
  float omega = 0.f, q = 10.7f;
  bank.init(init_freq, 8.f, SAMPLE_RATE);

  saw(saw_table, 512);
  hanning(hanning_table, 512);

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
	std::fprintf( stderr, "An error occurred omegahile using the portaudio stream\n" );
	std::fprintf( stderr, "Error number: %d\n", err );
	std::fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ));
	return err;
}
