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
Wavetable z1 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable v1 = Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none);
Wavetable v2 = Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none);
Wavetable v3 = Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none);
Wavetable v4 = Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none);
Wavetable v5 = Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none);
Biquad bq[2];
Biquad4 bq4;
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
    float sigl = z1.play(); // + z2.play() + z3.play() + z4.play();
    float sigr = sigl;
    for (auto &b: bq) {
      sigl = b.process(sigl);
    }
    sigr = bq4.process(sigr);
    *out++ = sigl;
    *out++ = sigr;
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
      for (auto &b: bq) {
        b.calcLPF(w, q);
      }
      bq4.calcLPF(w, q);
    } else if (!strcmp("2", argv[1])) {
      freq = 2500.f;
      w = TAU * freq / SAMPLE_RATE;
      for (auto &b: bq) {
        b.calcHPF(w, q);
      }
      bq4.calcHPF(w, q);
    } else if (!strcmp("3", argv[1])) {
      freq = zfreq * 5.f;
      w = TAU * freq / SAMPLE_RATE;
      for (auto &b: bq) {
        b.calcBPF(w, q);
      }
      bq4.calcBPF(w, q);
    } else if (!strcmp("4", argv[1])) {
      freq = 1000.f;
      w = TAU * freq / SAMPLE_RATE;
      for (auto &b: bq) {
        b.calcNotch(w, q);
      }
      bq4.calcNotch(w, q);
    }
  } else {
    for (auto &b: bq) {
      b.calcLPF(w, q);
    }
    bq4.calcLPF(w, q);
  }

	PaStream* stream;
	PaError err;
  z1.frequency = zfreq;
  v1.frequency = 1.f;
  v2.frequency = 1.5f;
  v3.frequency = 2.f;
  v4.frequency = 2.5f;
  v5.frequency = 3.f;

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
