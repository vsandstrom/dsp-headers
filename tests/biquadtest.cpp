#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../portaudio/include/portaudio.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/filter.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/waveshape.h"

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
Wavetable z1 = Wavetable::init(SAMPLE_RATE);
float table[512] {0.f};
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
    float sigl = z1.play<512, interpolation::hermetic>(table, 200.f, 0.f) * 0.05;
    float sigr = sigl;
    for (auto &b: bq) {
      sigl = b.process(sigl, 1.0);
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
      auto c = BiquadCoeffs::calc_lpf(w, q);
      for (auto &b: bq) b.set_coeffs(&c);
      bq4.set_coeffs(&c);

    } else if (!strcmp("2", argv[1])) {
      freq = 2500.f;
      w = TAU * freq / SAMPLE_RATE;
      auto c = BiquadCoeffs::calc_hpf(w, q);
      for (auto &b: bq) b.set_coeffs(&c);
      bq4.set_coeffs(&c);

    } else if (!strcmp("3", argv[1])) {
      freq = zfreq * 5.f;
      w = TAU * freq / SAMPLE_RATE;
      auto c = BiquadCoeffs::calc_bpf(w, q);
      for (auto &b: bq) b.set_coeffs(&c);
      bq4.set_coeffs(&c);

    } else if (!strcmp("4", argv[1])) {
      freq = 1000.f;
      w = TAU * freq / SAMPLE_RATE;
      auto c = BiquadCoeffs::calc_notch(w, q);
      for (auto &b: bq) b.set_coeffs(&c);
      bq4.set_coeffs(&c);

    }
  } else {
    auto c = BiquadCoeffs::calc_lpf(w, q);
    for (auto &b: bq) b.set_coeffs(&c);
    bq4.set_coeffs(&c);
  }

	PaStream* stream;
	PaError err;
  saw(table, 512);

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
