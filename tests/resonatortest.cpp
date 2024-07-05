#include <cstddef>
#include <cstdio>
#include "../portaudio/include/portaudio.h"
#include "../dsp/filter.hpp"
#include "../dsp/filterbank.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/interpolation.hpp"


// SETUP
constexpr int INPUT_CH = 2;
constexpr int OUTPUT_CH = 2;

constexpr int PROGRAM_DURATION = 120000; // milliseconds
constexpr float  SAMPLE_RATE =   48000;

using namespace dspheaders;

// GLOBALS
FilterBank8<5> bank;
Wavetable z1 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable va[5] = {
  Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none),
  Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none),
  Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none),
  Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none),
  Wavetable(HANNING, 512, SAMPLE_RATE, interpolation::none)
};

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
    float sig = z1.play();
    for (size_t i = 0; i < bank.size(); i++) {
      bank.setAmp(i, va[i].play());
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
  z1.frequency = init_freq;
  va[0].frequency = 0.2f;
  va[1].frequency = 0.32f;
  va[2].frequency = 0.54f;
  va[3].frequency = 0.89f;
  va[4].frequency = 1.11f;
 //  // Octave
 //  float freq = init_freq * 2.f;
 //  omega = TAU * freq / SAMPLE_RATE;
 //  b1.calcBPF(omega, q);
 //  b2.calcBPF(omega, q);
 //  b3.calcBPF(omega, q);
 //  b4.calcBPF(omega, q);
 //
 //  // Octave + Octave + Major Third
 //  freq = init_freq * 5.f;
 //  omega = TAU * freq  / SAMPLE_RATE;
 //  b5.calcBPF(omega, q);
 //  b6.calcBPF(omega, q);
 //  b7.calcBPF(omega, q);
 //  b8.calcBPF(omega, q);
 // 
 //  // Octave + Perfect Fifth
 //  freq = init_freq * 3.f;
 //  omega = TAU * freq  / SAMPLE_RATE;
 //  b9.calcBPF(omega, q);
 //  b10.calcBPF(omega, q);
 //  b11.calcBPF(omega, q);
 //  b12.calcBPF(omega, q);
 // 
 //  // octave + Octave + Minor Seventh
 //  freq = init_freq * 7.f;
 //  omega = TAU * freq  / SAMPLE_RATE;
 //  b13.calcBPF(omega, q);
 //  b14.calcBPF(omega, q);
 //  b15.calcBPF(omega, q);
 //  b16.calcBPF(omega, q);

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
