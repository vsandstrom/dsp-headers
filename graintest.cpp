#include "dsp/envelope.hpp"
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
#include "dsp/waveshape.h"

// SETUP
const int INPUT_CH = 2;
const int OUTPUT_CH = 2;
const int MAX_GRAINS = 8;
const float GRAIN_DUR = 0.01f;
const float INTERVAL = 0.1f;
const float RATE = 1.f; // TRY NEGATIVE
const float JITTER = 0.01f;
const float PITCH_MOD_AMOUNT = 0.05f;
const float RECORD_LEN = 16.f; // seconds

const int PROGRAM_DURATION = 60000; // milliseconds
const float  SAMPLE_RATE =   48000;

using namespace dspheaders;

// GLOBALS
Granulator* gr;
Envelope* genv;
Buffer buf = Buffer(RECORD_LEN, SAMPLE_RATE, interpolation::linear);
// Impulse trigger = Impulse(INTERVAL, SAMPLE_RATE);
Dust trigger = Dust(INTERVAL, SAMPLE_RATE);
Wavetable ph_saw = Wavetable(SAW, 1024, SAMPLE_RATE, interpolation::linear);
Wavetable lfo = Wavetable(SINE, 1024, SAMPLE_RATE, interpolation::linear);

float p[4] = { 0.01f, 0.8f, 2.2f, 0.3f };
float t[3] = { 12.f, 12.f, 18.f };
float c[3] = { 0.8f, 0.2f, 1.2f };


Envelope size = Envelope(p, 4, t, 3, c, 3, SAMPLE_RATE, interpolation::linear);

bool toggle = true;

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
  ph_saw.frequency = 1.f/24;
  lfo.frequency = 11.f;

  float gryn = 0.f;
  float s = 0.f;


	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    if (writeptr < buf.bufferlength) {
      float input = *in++;
      buf.writesample(input, writeptr % buf.bufferlength);
      writeptr++;
      in++; // skip one since input buffer is stereo
      *out++ = 0.f;
      *out++ = 0.f;
    } else {
      if (toggle || size.finished()) {
        s = size.play(GATE::on);
        toggle = false;
      } else {
        s = size.play(GATE::off);
      }
      
        float trig = trigger.play();
        float phasor = map(ph_saw.play(),-1.f, 1.f, 0.f, 0.99f);
        gr->setGrainSize(s);
        gryn = gr->process(
          phasor, // TRY STATIC VALUE (0.0 <= x < 1.0)
          RATE + (lfo.play() * PITCH_MOD_AMOUNT),
          trig
        ); 
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

  printf(
      "\n\n    ╒═════════════════════════════════════════════════════╕\n"
      "    │ THIS PROGRAM RECORDS THE INPUT OF YOUR AUDIO DEVICE │\n" 
      "    │ FOR A COUPLE OF SECONDS, THEN PLAYS THE AUDIO BACK  │\n" 
      "    │ THROUGH A GRANULATED AUDIO EFFECT.                  │\n"
      "    │                                                     │\n"
      "    │           ( NOTHING IS SAVED TO MEMORY )            │\n"
      "    ╘═════════════════════════════════════════════════════╛\n\n");

  // PLAY AROUND WITH CUSTOM ENV CURVES:
  //
  // float* envtable = new float[512];
  // hanning(envtable, 512);
  // range(envtable, 512, -1.01f, 1.01f, 0.01f, 0.99f);

  // genv = new Envelope(
  //     envtable,
  //     512,
  //     SAMPLE_RATE,
  //     interpolation::linear
  //   );

  // gr = new Granulator(
  //     GRAIN_DUR,
  //     SAMPLE_RATE,
  //     MAX_GRAINS,
  //     envtable,
  //     512,
  //     interpolation::linear,
  //     &buf
  //   );

  gr = new Granulator(
      GRAIN_DUR,
      SAMPLE_RATE,
      MAX_GRAINS,
      &buf,
      interpolation::linear
    );

  gr->setJitter(JITTER);
  gr->setNumGrains(MAX_GRAINS);
  
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
