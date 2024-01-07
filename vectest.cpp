#include "dsp/dsp.h"
#include "portaudio/include/portaudio.h"
#include <cstdio>
#include <iostream>
#include <string>
#include "dsp/wavetable.hpp"
#include "dsp/vectoroscillator.hpp"
#include "dsp/interpolation.hpp"
#include <vector>

struct frame {
  float left;
  float right;
};


// MASTER VOLUME OF THE GENERATED TONE
const float AMP =              1.0f;
// DURATION OF THE GENERATED TONE
const int DURATION =           10000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int TABLE_LEN =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

static frame data;

using namespace dspheaders;

// SETUP:
Wavetable sine = Wavetable(SINE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable triangle = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable square = Wavetable(SQUARE, TABLE_LEN, SAMPLE_RATE, interpolation::none);
Wavetable saw = Wavetable(SAW, TABLE_LEN, SAMPLE_RATE, interpolation::linear);
Wavetable transfer = Wavetable(SAW, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable envelope = Wavetable(HANNING, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
std::vector<Wavetable> vecTables = {sine, triangle, square, saw};
VectorOscillator vec = VectorOscillator(vecTables, interpolation::cubic);

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(  const void* inputBuffer,	
						void* outputBuffer,								      
						unsigned long framesPerBuffer,					 
						const PaStreamCallbackTimeInfo* timeinfo,	
						PaStreamCallbackFlags statusFlags,         
						void* userdata )								            
{

	// cast data passing through stream
	float* out = (float*)outputBuffer;
	unsigned int i;
	(void) inputBuffer; // prevent unused variable warning

	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    float vosc = vec.play( map( transfer.play(), -1.f, 1.f, 0.f, 1.f)) * envelope.play();
    *out++ = clamp(vosc, -1.f, 1.f); 
    *out++ = clamp(vosc, -1.f, 1.f); 
	}
	return 0;
}

int main(int argc, char** argv) {
  vec.frequency = FREQ;
  envelope.frequency = ENV_FREQ;
    if ( argc > 3 && argc < 8 ) {
      argc--;
      argv++;
      while (argc > 0){
        if ((*argv)[0] == '-') {
          printf("%c\n", (*argv)[1]);
          switch ((*argv)[1]){
            case 'v': {
              argc--;
              argv++;
              // carrier.frequency = std::stof(*argv);
              vec.frequency = std::stof(*argv);
              break;
            }
            case 'e':{
              argc--;
              argv++;
              envelope.frequency = std::stof(*argv);
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

  // transfer is used as guide wave to determine where between the tables in the vectoroscillator
  // we want to read.
  // This readpointer needs to be positive
  transfer.frequency = 0.2f;

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



