#include <cstddef>
#include <cstdio>
#include <iostream>
#include "../portaudio/include/portaudio.h"
#include "../dsp/dsp.h"
#include "../dsp/vectortable.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/interpolation.hpp"

const float AMP               = 1.0f;
const int DURATION            = 25000; // milliseconds
const size_t SIZE             = 512;
const size_t WIDTH            = 4;
const float  SAMPLE_RATE      = 48000;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ                    = 200.0f;
float FM_FREQ                 = 180.0f;
float ENV_FREQ                = 4.0f;

static frame data;

using namespace dspheaders;

// SETUP:
VecOsc vec = VecOsc<interpolation::linear>::init(SAMPLE_RATE);
Osc transfer = Osc<interpolation::linear>::init(SAMPLE_RATE);
Osc envelope = Osc<interpolation::linear>::init(SAMPLE_RATE);

// Tables
// 
float* tables[WIDTH]   = {nullptr};
float trans[SIZE+1]    = {0.f};
float env[SIZE+1]      = {0.f};
float t0[SIZE+1]       = {0.f};
float t1[SIZE+1]       = {0.f};
float t2[SIZE+1]       = {0.f};
float t3[SIZE+1]       = {0.f};


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
    float vosc = vec.play<SIZE, WIDTH>(
        (const float**)tables,
        FREQ,
        map(transfer.play(
          trans,
          SIZE,
          0.45,
          0.f), -1.f, 1.f, 0.f, 1.f),
        0.f
        // )
      // * envelope.play<SIZE, interpolation::linear>(
      //   env,
      //   0.2,
      //   0.f
      );
    *out++ = clamp(vosc, -1.f, 1.f); 
    *out++ = clamp(vosc, -1.f, 1.f); 
	}
	return 0;
}

int main(int argc, char** argv) {
	PaStream* stream;
	PaError err;

  // fill float arrays with wavetable shapes
  // transform table
  sine(trans, SIZE);
  // envelope table
  hanning(env, SIZE);
  // vector table 1
  sine(t0, SIZE);
  // vector table 2
  float amp[2] = {1.f, 0.6f};
  float phs[2] = {0.f, 0.f};
  complex_sine(t1, SIZE, amp, phs, 2);
  // vector table 3
  float amp2[6] = {1.f, 0.3, 0.8f, 0.5f, 0.1f, 0.3f};
  float phs2[6] = {0.f, 0.3f, 0.6f, 0.1f, 0.8f, 0.75f};
  complex_sine(t2, SIZE, amp2, phs2, 2);
  // vector table 4
  saw(t3, SIZE);

  tables[0] = t0;
  tables[1] = t1;
  tables[2] = t2;
  tables[3] = t3;

  // initialize first value, no wierd garbage value
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



