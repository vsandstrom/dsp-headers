#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../portaudio/include/portaudio.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/filter.hpp"
#include "../dsp/wavetable.hpp"

#ifdef DEBUG
  #define D(x) x
#else
  #define D(x) 
#endif

// SETUP
constexpr int INPUT_CH = 2;
constexpr int OUTPUT_CH = 2;
const unsigned POLES = 2;

constexpr int PROGRAM_DURATION = 120000; // milliseconds
constexpr float  SAMPLE_RATE =   48000;

using namespace dspheaders;

// GLOBALS
Wavetable* lfo = nullptr;
Wavetable z1 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable z2 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable z3 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable z4 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);

Biquad b1 = Biquad();
Biquad b2 = Biquad();
Biquad b3 = Biquad();
Biquad b4 = Biquad();
Biquad bq[4] = {b1, b2, b3, b4};
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
    float sig = z1.play(); // + z2.play() + z3.play() + z4.play();
    sig *= 0.1;
    sig = b1.process(sig);
    sig = b2.process(sig);
    sig = b3.process(sig);
    sig = b4.process(sig);
    *out++ = sig;
    *out++ = sig;
	}
	return 0;
}

int main(int argc, char** argv) {
  float freq = 300.f;
  float zfreq = 127.f;
  float w = 0.f, q = 10.9707f;
  if (argc > 2) { freq = atof(argv[2]); }

  if (argc==2) {
    if (!strcmp("1", argv[1])) {
      w = TAU * freq / SAMPLE_RATE;
      b1.calcLPF(w, q);
      b2.calcLPF(w, q);
      b3.calcLPF(w, q);
      b4.calcLPF(w, q);
    } else if (!strcmp("2", argv[1])) {
      freq = 2500.f;
      w = TAU * freq / SAMPLE_RATE;
      b1.calcHPF(w, q);
      b2.calcHPF(w, q);
      b3.calcHPF(w, q);
      b4.calcHPF(w, q);
    } else if (!strcmp("3", argv[1])) {
      freq = zfreq * 5.f;
      w = TAU * freq / SAMPLE_RATE;
      b1.calcBPF(w, q);
      b2.calcBPF(w, q);
      b3.calcBPF(w, q);
      b4.calcBPF(w, q);
    } else if (!strcmp("4", argv[1])) {
      freq = 1000.f;
      w = TAU * freq / SAMPLE_RATE;
      b1.calcNotch(w, q);
      b2.calcNotch(w, q);
      b3.calcNotch(w, q);
      b4.calcNotch(w, q);
    }
  } else {
    b1.calcLPF(w, q);
    b2.calcLPF(w, q);
    b3.calcLPF(w, q);
    b4.calcLPF(w, q);
  }

	PaStream* stream;
	PaError err;

  z1.frequency = zfreq;
  z2.frequency = zfreq * 5/2;
  z3.frequency = zfreq * 3;
  z2.frequency = zfreq * 9/2;

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
