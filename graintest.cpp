#ifdef DEBUG
  #define D(x) x
#else
  #define D(x) 
#endif


#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <portaudio.h>
#include "dsp/dsp.h"
#include "dsp/interpolation.hpp"
#include "dsp/wavetable.hpp"
#include "dsp/grain.hpp"
#include "dsp/interpolation.hpp"
#include "dsp/trigger.hpp"

// SETUP
const int DURATION = 30000; // milliseconds
const float  SAMPLE_RATE =   48000;
const int INPUT_CH = 2;
const int OUTPUT_CH = 2;
using namespace dspheaders;

// GLOBALS
Granulator* gr;
Buffer buf = Buffer(8.f, SAMPLE_RATE, interpolation::linear);
// Impulse trigger = Impulse(0.1f, SAMPLE_RATE);
Dust trigger = Dust(0.1f, SAMPLE_RATE);
Wavetable saw = Wavetable(SAW, 1024, SAMPLE_RATE, interpolation::linear);
Wavetable lfo = Wavetable(SINE, 1024, SAMPLE_RATE, interpolation::linear);


// Duration in samples
int playhead = 0;
// buffer writeindex
int writeptr = 0;

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
	unsigned int i;
  saw.frequency = 1.f/24;
  lfo.frequency = 11.f;

  float gryn = 0.f;


	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    if (writeptr < buf.bufferlength) {
      float input = *in++;
      buf.writesample(input, writeptr % buf.bufferlength);
      writeptr++;
      in++; // skip one since input buffer is stereo
      *out++ = 0.f;
      *out++ = 0.f;
    } else {
        float trig = trigger.play();
        float phasor = map(saw.play(),-1.f, 1.f, 0.f, 0.99f);
          gryn = gr->process(
            phasor, 
            (lfo.play() * 0.05) - 1.f,
            trig
          ); 
        *out++ = gryn;
        *out++ = gryn;

        // gr->setRate((lfo.play()*0.05) - 1.f);
    }
    playhead++;
	}
	return 0;
}

int main(int argc, char** argv) {

  srand(time(NULL));
  gr = new Granulator(0.8f, SAMPLE_RATE, 32, &buf, interpolation::linear);
  gr->setJitter(0.02f);
  gr->setNumGrains(32);
  gr->setRate(0.8);
  
	PaStream* stream;
	PaError err;

  // Initialize from the beginning
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
	Pa_Sleep(DURATION); // NUM_SECONDS is in milliseconds????

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
