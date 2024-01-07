#include "dsp/dsp.h"
#include "portaudio/include/portaudio.h"
#include <cstdio>
#include <iostream>
#include "dsp/delay.hpp"
#include "dsp/interpolation.hpp"

// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           30000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
constexpr int TABLE_LEN =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000;

static float delaytime = 0.0f;
static float fb = 0.0f;

using namespace dspheaders;
Delay delay = Delay(SAMPLE_RATE, 4.f, 2, interpolation::cubic);

static frame data;

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(
            const void* inputBuffer,             				// input
						void* outputBuffer,								          // output
						unsigned long framesPerBuffer,					    // length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		//
						PaStreamCallbackFlags statusFlags,          //
						void* userdata )								            // "void"-type can be typecast to other 
{

	// cast data passing through stream
	float* out = (float*)outputBuffer;
	unsigned int i;

	float* in = (float*)inputBuffer;

    
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    // write and increment output and input buffer simultaneously. 
    // hardcoded for a stereo i/o setup
    *out++ = delay.process(*in++, delaytime, 0.8f, fb); 
    *out++ = delay.process(*in++, delaytime, 0.8f, fb); 
	}
	return 0;
}

int main(int argc, char** argv) {
  // transfer is used as guide wave to determine where between the tables in the vectoroscillator
  // we want to read.
  // This readpointer needs to be positive
  // transfer.frequency = 0.2f;
    if ( argc > 2 && argc <= 5 ) {
      argc--;
      argv++;
      while (argc > 0){
        if ((*argv)[0] == '-') {
          printf("%c\n", (*argv)[1]);
          switch ((*argv)[1]){
            case 't': {
              argc--;
              argv++;
              // carrier.frequency = std::stof(*argv);
              delaytime = std::stof(*argv);
              break;
            }
            case 'f':{
              argc--;
              argv++;
              fb = std::stof(*argv);
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
      printf("running user input settings\n");
    } else {
      printf("running on default settings\n");
    }

	PaStream* stream;
	PaError err;

  // initialize first value, no wierd garbage value
  // if they are initialized here, make sure to give the variables the correct values
  // before using it, otherwise there will be an unwanted '0'-sample at the first block
	data.left = data.right = 0.0f;

	err = Pa_Initialize();
	if ( err != paNoError ) goto error;

	// open an audio I/O stream:
	err = Pa_OpenDefaultStream( &stream,  // < --- Callback is in err
								2, 
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
