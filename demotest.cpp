#include "dsp/interpolation.hpp"
#include "portaudio/include/portaudio.h"
#include <cstdio>
#include <iostream>
#include "dsp/wavetable.hpp"
#include "dsp/envelope.hpp"
#include "dsp/delay.hpp"

// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           30000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int TABLE_LEN =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000.f;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

float breakpoints[] = {0.f, 0.8f, 0.3f, 0.f};
float breaktimes[] = {0.01f, 0.1f, 0.4};

// Fundamental pitch
float fund = 200.f;
// Pitch score
float score[] = {
  fund * 0.8f, 
  fund, 176.f * 2.0f, 
  fund/3.f, 
  fund*4.f/1.5f, 
  fund/3.f, 
  176.f/3.f, 
  fund*9/4, 
  fund*9/5.f,
  fund*4/3.f,
  176.f/3.f, 
};
// Duration before retriggering the envelope, in seconds from start
float dur[] = {1.2, 1.4, 2.0, 2.2, 2.6, 3.8, 4.0, 4.1, 4.2, 4.3, 4.7, 5.1, 5.3, 5.6, 5.7, 6.2, 6.4, 7.2, 7.6, 8.0, 8.6};

// keeps track of the number of the current sample
unsigned timeline = 0;
// Progresses the score
unsigned scoreptr = 0;

using namespace dspheaders;
Envelope envelope = Envelope(breakpoints, 5, breaktimes, 4, SAMPLE_RATE, interpolation::linear);
Wavetable carrier = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable modulator = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable vib = Wavetable(SINE, TABLE_LEN, SAMPLE_RATE, interpolation::linear);
Delay delay = Delay(SAMPLE_RATE, 4.f, 4, interpolation::cubic);

static frame data;

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(  const void* inputBuffer,				// input
						void* outputBuffer,								          // output
						unsigned long framesPerBuffer,					    // length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		//
						PaStreamCallbackFlags statusFlags,          //
						void* userdata )								            // "void"-type can be typecast to other 
{

	// cast data passing through stream
	frame* data = (frame*) userdata;
	float* out = (float*)outputBuffer;
	unsigned int i;

  float env = 0.f;

	(void) inputBuffer; // prevent unused variable warning

	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer

    if ( timeline == (int)(48000 * dur[scoreptr % 18])) {
      env = envelope.play(GATE::on);
      carrier.frequency = score[scoreptr % 11];
      modulator.frequency = score[scoreptr % 7] * 7/2; 
      scoreptr++;
    } else {
      env = envelope.play(GATE::off);
    }
    float car = carrier.play(modulator.play()+(vib.play() * 0.01));
    float sig = car*env;
    sig += delay.play(sig, 0.8, 0.2, 0.01f);

    // Stereo frame: two increments of out buffer
    *out++ = sig; 
    *out++ = sig;

    timeline++;
	}
	return 0;
}

int main(int argc, char** argv) {
  carrier.frequency = FREQ;
  modulator.frequency = FM_FREQ;
  vib.frequency = 3.2f;
    if ( argc > 3 && argc < 8 ) {
      argc--;
      argv++;
      while (argc > 0){
        if ((*argv)[0] == '-') {
          printf("%c\n", (*argv)[1]);
          switch ((*argv)[1]){
            case 'c': {
              argc--;
              argv++;
              // carrier.frequency = std::stof(*argv);
              carrier.frequency = std::stof(*argv);
              break;
            }
            case 'm':{
              argc--;
              argv++;
              modulator.frequency = std::stof(*argv);
              break;
            }
            default:{
              argc--;
              argv++;
              break;

            }
          }
        }
        argc--;
        argv++;
      }
      printf("running user input frequencies\n");
    } else {
      printf("running on default frequencies\n");
    }

	PaStream* stream;
	PaError err;

	data.left = data.right = 0.0f;

	err = Pa_Initialize();
	if ( err != paNoError ) goto error;

	// open an audio I/O stream:
	err = Pa_OpenDefaultStream( &stream,  // < --- Callback is in err
								0, 
								2,
								paFloat32,
								(int)SAMPLE_RATE,
							  256, 
								paCallback,
								&data
			);

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
	std::cout << "Test Finished.\n";
	return err;
error:
	Pa_Terminate();
	std::fprintf( stderr, "An error occurred while using the portaudio stream\n" );
	std::fprintf( stderr, "Error number: %d\n", err );
	std::fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ));
	return err;
}
