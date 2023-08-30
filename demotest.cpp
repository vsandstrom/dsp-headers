#include <cstdio>
#include <portaudio.h>
#include <vector>
#include "dsp/interpolation.hpp"
#include "dsp/wavetable.hpp"
#include "dsp/envelope.hpp"
#include "dsp/verb.hpp"
#include "dsp/delay.hpp"
#include "dsp/waveshape.h"
#include "dsp/vectoroscillator.hpp"

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

// Fundamental pitch
float fund = 200.f;
// Pitch score
float score[11] = {
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
// float dur[21] = {1.2, 1.4, 2.0, 2.2, 2.6, 3.8, 4.0, 4.1, 4.2, 4.3, 4.7, 5.1, 5.3, 5.6, 5.7, 6.2, 6.4, 7.2, 7.6, 8.0, 8.6};
float dur[21] = {1.2, 0.2, 0.6, 0.2, 0.4, 1.2, 0.2, 0.1, 0.1, 0.1, 0.4, 0.4, 0.2, 0.3, 0.1, 0.5, 0.2, 0.8, 0.4, 0.4, 0.6 };

// keeps track of the number of the current sample
unsigned timeline = 0;
// Progresses the score
unsigned scoreptr = 0;
unsigned seq = (unsigned)(dur[scoreptr] * SAMPLE_RATE);

using namespace dspheaders;

// VectorOscillator tables
float amp0[] = {1};
float phs0[] = {0};
float table0[513] = {0.0f};
Wavetable *w0, *w1, *w2, *w3;


float amp1[] = {1, 0.3, 0.2};
float phs1[] = {0, 0.1, 0.8};
float table1[513] = {0.0f};

float amp2[] = {1, 0.1, 0.2, 0.5, 0.3};
float phs2[] = {0, 0.1, 0.4, 0.5, 0.6};
float table2[513] = {0.0f};

float amp3[] = {1, 0, 0.6, 0.2, 0.4, 0.1, 0.25};
float phs3[] = {0, 0, 0.2, 0.45, 0.2, 0, 0.7};
float table3[513] = {0.0f};

VectorOscillator *vec;


//  Volume Envelope
float ap[] = {0.f, 0.8f, 0.3f, 0.f};
float at[] = {0.01f, 0.1f, 0.4};
Envelope ampenv = Envelope(ap, 4, at, 3, SAMPLE_RATE, interpolation::linear);

// Vector Movement Envelope
float vp[] = {0.f, 0.8f, 0.3f, 0.f};
float vt[] = {1.1f, 0.8f, 1.4f};
Envelope vecenv = Envelope(vp, 4, vt, 3, SAMPLE_RATE, interpolation::linear);

// Wavetable carrier = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
// Wavetable* modulator 
Wavetable modulator = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable vib = Wavetable(SINE, TABLE_LEN, SAMPLE_RATE, interpolation::linear);
Delay delay = Delay(SAMPLE_RATE, 4.f, 4, interpolation::cubic);
// Verb verb = Verb(SAMPLE_RATE, 0.4f, interpolation::linear);

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
  float venv = 0.f;

	(void) inputBuffer; // prevent unused variable warning
                      //

	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    if ( timeline == seq ) {
      scoreptr++;
      seq += (unsigned)(SAMPLE_RATE * dur[scoreptr % 18]);
      env = ampenv.play(GATE::on);
      venv = vecenv.play(GATE::on);
      vec -> frequency = score[scoreptr % 11];
      modulator.frequency = score[scoreptr % 7] * 7/2; 
    } else {
      env = ampenv.play(GATE::off);
      venv = vecenv.play(GATE::off);
    }
    float car = vec -> play(venv, map(modulator.play()+(vib.play() * 0.01), -1.f, 1.f, 0.f, 1.f));
    // float car = carrier.play(modulator.play()+(vib.play() * 0.01));
    float sig = car*env;
    sig += delay.play(sig, 0.8, 0.2, 0.01f);
    // sig += verb.play(sig) * 0.5;

    // Stereo frame: two increments of out buffer
    *out++ = sig; 
    *out++ = sig;

    timeline++;
	}
	return 0;
}

int main(int argc, char** argv) {

  // Initialize a waveform
  w0 = new Wavetable(complex_sine(table0, 512, amp0, phs0, 1) , 512, SAMPLE_RATE, interpolation::cubic);
  w1 = new Wavetable(complex_sine(table1, 512, amp1, phs1, 3) , 512, SAMPLE_RATE, interpolation::cubic);
  w2 = new Wavetable(complex_sine(table2, 512, amp2, phs2, 5) , 512, SAMPLE_RATE, interpolation::cubic);
  w3 = new Wavetable(complex_sine(table3, 512, amp3, phs3, 7) , 512, SAMPLE_RATE, interpolation::cubic);

  std::vector<Wavetable> t = {*w0, *w1, *w2, *w3};
  vec = new VectorOscillator(t, interpolation::cubic);

  vec->frequency = FREQ;
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
              vec -> frequency = std::stof(*argv);
              // carrier.frequency = std::stof(*argv);
              break;
            }
            case 'm':{
              argc--;
              argv++;
              // modulator -> frequency = std::stof(*argv);
              modulator.frequency = std::stof(*argv);
              break;
            }
            case 'f':{
              argc--;
              argv++;
              // verb.feedback(std::stof(*argv));
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
