#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../portaudio/include/portaudio.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/filter.hpp"
#include "../dsp/wavetable.hpp"

// SETUP
constexpr int INPUT_CH = 2;
constexpr int OUTPUT_CH = 2;

constexpr int PROGRAM_DURATION = 120000; // milliseconds
constexpr float  SAMPLE_RATE =   48000;

using namespace dspheaders;

// GLOBALS
Wavetable* lfo = nullptr;
Wavetable z1 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable z2 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable z3 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);
Wavetable z4 = Wavetable(SAW, 512, SAMPLE_RATE, interpolation::none);

Biquad b1 {Biquad()};
Biquad b2 {Biquad()};
Biquad b3 {Biquad()};
Biquad b4 {Biquad()};

Biquad b5 {Biquad()};
Biquad b6 {Biquad()};
Biquad b7 {Biquad()};
Biquad b8 {Biquad()};

Biquad b9 {Biquad()};
Biquad b10 {Biquad()};
Biquad b11 {Biquad()};
Biquad b12 {Biquad()};

Biquad b13 {Biquad()};
Biquad b14 {Biquad()};
Biquad b15 {Biquad()};
Biquad b16 {Biquad()};

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
    float sig = *in++;
    void* _ = in++;

      // z1.play(); // + z2.play() + z3.play() + z4.play();
    // sig *= 0.1;
    float s1 = sig, s2 = sig, s3 = sig, s4 = sig;
    s1 = b1.process(s1);
    s1 = b2.process(s1);
    s1 = b3.process(s1);
    s1 = b4.process(s1);

    s2 = b5.process(s2);
    s2 = b6.process(s2);
    s2 = b7.process(s2);
    s2 = b8.process(s2);
    
    s3 = b9.process(s3);
    s3 = b10.process(s3);
    s3 = b11.process(s3);
    s3 = b12.process(s3);
    
    s4 = b13.process(s4);
    s4 = b14.process(s4);
    s4 = b15.process(s4);
    s4 = b16.process(s4);
    *out++ = (s1 + s2 + s3 + s4) / 4;
    *out++ = (s1 + s2 + s3 + s4) / 4;
	}
	return 0;
}

int main(int argc, char** argv) {
  float freq = 300.f;
  float zfreq = 127.f;

  float omega = 0.f, q = 5.f;

  freq = zfreq * 2.f;
  omega = TAU * freq / SAMPLE_RATE;
  b1.calcBPF(omega, q);
  b2.calcBPF(omega, q);
  b3.calcBPF(omega, q);
  b4.calcBPF(omega, q);

  freq = zfreq * 5.f;
  omega = TAU * freq  / SAMPLE_RATE;
  b5.calcBPF(omega, q);
  b6.calcBPF(omega, q);
  b7.calcBPF(omega, q);
  b8.calcBPF(omega, q);
  
  freq = zfreq * 3.f;
  omega = TAU * freq  / SAMPLE_RATE;
  b9.calcBPF(omega, q);
  b10.calcBPF(omega, q);
  b11.calcBPF(omega, q);
  b12.calcBPF(omega, q);
  
  freq = zfreq * 7.f;
  omega = TAU * freq  / SAMPLE_RATE;
  b13.calcBPF(omega, q);
  b14.calcBPF(omega, q);
  b15.calcBPF(omega, q);
  b16.calcBPF(omega, q);

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
	std::fprintf( stderr, "An error occurred omegahile using the portaudio stream\n" );
	std::fprintf( stderr, "Error number: %d\n", err );
	std::fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ));
	return err;
}
