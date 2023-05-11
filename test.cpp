#include "portaudio.h"

#include "dsp/dsp.h"
#include "dsp/wavetable.hpp"
#include "dsp/vectoroscillator.hpp"


#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <string>
// #include "sound.hpp"
#include <vector>

using namespace dspheaders;

struct frame {
  float left;
  float right;
};


// DURATION OF THE GENERATED TONE
const float DURATION = 5000.f; // milliseconds
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const int SAMPLE_RATE = 48000;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =           200.0f;
float FM_FREQ =        180.0f;
float ENV_FREQ =       4.0f;

WaveTable mod = WaveTable(SAMPLE_RATE);
VectorOscillator vec = VectorOscillator(SAMPLE_RATE);

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

	(void) inputBuffer; // prevent unused variable warning

    
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    // float mono = carrier.interpolate() * envelope.interpolate();
    // float car = carrier.play(modulator.play());
    // float env = envelope.play();
    float temp = clamp(vec.play(scale(mod.play(), -1.f, 1.f, 0.f, 1.f)), -1.f, 1.f);
    *out++ = temp; 
    *out++ = temp;

    // the modulator modulates the carriers phase
    // carrier.movePointer(modulator.interpolate());
    // modulator.movePointer();
    // envelope.movePointer(); 

	}
	return 0;
}


int main(int argc, char** argv) {

  mod.setFreq(0.05);
  mod.setWave(TRIANGLE);
  vec.setFreq(200);
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
              vec.setFreq(std::stof(*argv));
              break;
            }
            case 'e':{
              argc--;
              argv++;
    //          envelope.setFreq(std::stof(*argv));
              break;
            }
            // case 'm':{
            //   argc--;
            //   argv++;
            //   modulator.frequency = std::stof(*argv);
            //   break;
            // }
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



