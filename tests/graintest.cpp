#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "../portaudio/include/portaudio.h"
#include "../dsp/dsp.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/wavetable_ex.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/grain_ex.hpp"
#include "../dsp/interpolation.hpp"
#include "../dsp/trigger.hpp"

#ifdef DEBUG
  #define D(x) x
#else
  #define D(x) 
#endif

// SETUP
const int INPUT_CH = 2;
const int OUTPUT_CH = 2;
const float INTERVAL = 0.1f;
const float RECORD_LEN = 4.f; // seconds

const int PROGRAM_DURATION = 120000; // milliseconds
const float  SAMPLE_RATE =   48000;
const unsigned SIZE = 512;

using namespace dspheaders;
using namespace interpolation;

GranulatorEX<16, 4*48000> gr = GranulatorEX<16, 4*48000>::init(SAMPLE_RATE);

// GLOBALS
Impulse trigger = Impulse(SAMPLE_RATE);
Wavetable phase = Wavetable::init(SAMPLE_RATE);
float table[512] = {0.f};

// Dust trigger = Dust(INTERVAL, SAMPLE_RATE);


// Duration in samples
int playhead = 0;
// buffer writeindex
int writeptr = 0;

static frame data;
D(static size_t main_count = 0;)

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
	const float* in = (float*)inputBuffer;
	unsigned int i;

  float gryn = 0.f;
  float s = 0.f;
  float r = 0.f;
  float t = 0.1f;


	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    if (gr.record(*in++)) {
      in++;
    } else {
        float trig = trigger.play(t);
        float ph   = phase.play<SIZE, linear>(table, 0.25, 0.f);

        D(if (trig >= 0.5f) printf("main: %zu", main_count++));
        // printf("phaser position: %f\n", phasor);
        gryn = gr.play<linear, linear>(
            ph,
            2.f,
            1.f,
            0.f,
            trig
        ) * 0.2; 
        *out++ = gryn;
        *out++ = gryn;
    }
    playhead++;
	}
	return 0;
}

int main(int argc, char** argv) {
  // seed rng for dust  and jitter in graulator object
  srand(time(NULL));

  phasor(table, SIZE);

  printf(
      "\n\n    ╒═════════════════════════════════════════════════════╕\n"
          "    │ THIS PROGRAM RECORDS THE INPUT OF YOUR AUDIO DEVICE │\n" 
          "    │ FOR A %*.*f OF SECONDS, THEN PLAYS THE AUDIO BACK     │\n" 
          "    │ THROUGH A GRANULATED AUDIO EFFECT.                  │\n"
          "    │                                                     │\n"
          "    │           ( NOTHING IS SAVED TO MEMORY )            │\n"
          "    ╘═════════════════════════════════════════════════════╛\n\n", 1, 1, RECORD_LEN);

  // PLAY AROUND WITH CUSTOM ENV CURVES:
  // float env[512] = {0.f};
  // hanning(env, 512);

  // gr.update_envelope(env, 512);


  
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
