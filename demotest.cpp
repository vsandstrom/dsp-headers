#include <portaudio.h>
#include <vector>
#include "dsp/dsp.h"
#include "dsp/interpolation.hpp"
#include "dsp/wavetable.hpp"
#include "dsp/envelope.hpp"
#include "dsp/verb.hpp"
#include "dsp/delay.hpp"
#include "dsp/filter.hpp"
#include "dsp/waveshape.h"
#include "dsp/vectoroscillator.hpp"

// DURATION OF THE GENERATED TONE
const int DURATION =           120000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int TABLE_LEN =      512;
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =   48000.f;

// CHANGE THE VALUES BELOW FOR OTHER PITCHES 
float FREQ =                300.0f;
float FM_FREQ =             180.0f;
float ENV_FREQ =              4.0f;

// Fundamental pitch
float fund = 80.f;
// Pitch score
float score[] = {
  fund, fund * 4/3, fund * 3/2*3, fund * 12/7*2, fund * 9/8, 
  fund * 3, fund * 4/3, fund * 12/7*2, fund * 9/8, 
  fund * 3, fund * 11/10*3, fund * 8/3*3,
};

float amps[] = {
  0.8, 0.45, 0.3, 0.5, 
  0.75, 0.5, 0.65, 0.3, 0.4
};

// Duration before retriggering the envelope, in seconds from start
// float dur[21] = {1.2, 1.4, 2.0, 2.2, 2.6, 3.8, 4.0, 4.1, 4.2, 4.3, 4.7, 5.1, 5.3, 5.6, 5.7, 6.2, 6.4, 7.2, 7.6, 8.0, 8.6};
float dur[21] = {0.4, 0.4, 0.6, 0.2, 0.8, 0.2, 0.4, 0.4, 0.2, 0.2, 0.4, 0.4, 0.2, 0.3, 0.1, 0.5, 0.2, 0.8, 0.4, 0.4, 0.6 };

float score1[] {
  fund * 3/2, fund * 6/5/3, fund / 3, fund * 6/5
};

float amps1[] = {
  0.6, 0.5, 0.3, 0.45
};

float dur1[] = {
  0.8, 1.2, 0.8, 0.8, 0.4
};


// keeps track of the number of the current sample
unsigned timeline = 0;
// Progresses the score
unsigned scoreptr = 0;
unsigned scoreptr1 = 0;
unsigned seq = (unsigned)(dur[scoreptr] * SAMPLE_RATE);
unsigned seq1 = (unsigned)(dur1[scoreptr1] * SAMPLE_RATE);

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

VectorOscillator *vec, *vec1;
//  Volume Envelope
// float ap[] = {0.f, 0.5f, 0.3f, 0.f};
// float at[] = {0.6f, 1.2f, 0.4};
// float ac[] = {2.6f, 1.2f, 3.4};
float ap[] = {0.f, 0.5f, 0.f};
float at[] = {2.01f, 2.5f};
float ac[] = {0.1f, 1.6};
Envelope ampenv = Envelope(ap, 3, at, 2, ac, 2, SAMPLE_RATE, interpolation::cubic);
Envelope ampenv1 = Envelope(ap, 3, at, 2, ac, 2, SAMPLE_RATE, interpolation::cubic);

// Vector Movement Envelope
// float vp[] = {.0f, .8f, .3f, .0f};
// float vt[] = {1.1f, 0.8f, 1.4f};
// float vc[] = {.6f, .8f, 1.4f};
float vp[] = {.0f, .8f, .0f};
float vt[] = {.21f, .6f};
float vc[] = {1.2f, .4f};
Envelope vecenv = Envelope(vp, 3, vt, 2, SAMPLE_RATE, interpolation::linear);
Envelope vecenv1 = Envelope(vp, 3, vt, 2, SAMPLE_RATE, interpolation::linear);

// Wavetable carrier = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
// Wavetable* modulator 
Wavetable modulator = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable modulator1 = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
Wavetable vib = Wavetable(SINE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);

Delay delay = Delay(SAMPLE_RATE, 4.f, 1, interpolation::linear);
SchroederVerb verb = SchroederVerb(SAMPLE_RATE);

static frame data;

float a[16] = {
   1,  1,  1,  1,
  -1, -1, -1, -1,
  -1,  1, -1,  1,
   1, -1,  1, -1
};

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
  float env1 = 0.f;
  float venv1 = 0.f;
  float rev = 0.f;
  float del = 0.f;

  unsigned x = 0;

	(void) inputBuffer; // prevent unused variable warning
                      //

	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
                                          //
    // Modultion and sequencer section
    if ( timeline == seq ) {
      scoreptr++;
      seq += (unsigned)(SAMPLE_RATE * dur[scoreptr % 13]);
      env = ampenv.play(GATE::on);
      venv = vecenv.play(GATE::on);
      vec -> frequency = score[scoreptr % 13];
      modulator.frequency = score[scoreptr % 5] * 3; 
    } else {
      env = ampenv.play(GATE::off, 2.f);
      venv = vecenv.play(GATE::off, 3.f);
    }

    if (timeline == seq1) {
      scoreptr++;
      seq1 += (unsigned)(SAMPLE_RATE * dur1[scoreptr1 % 5]);
      env1 = ampenv1.play(GATE::on);
      venv1 = vecenv1.play(GATE::on);
      vec1 -> frequency = score1[scoreptr1 % 4];
      modulator1.frequency = score1[scoreptr1 % 4] * 3;
    } else {
      env1 = ampenv1.play(GATE::off, 2.f);
      venv1 = vecenv1.play(GATE::off, 3.f);
    }

    float vibr = vib.play();

    float mod0 = modulator.play();
    float mod1 = modulator.play();

    // Sound generation section
    float car = vec -> play(venv, map(mod0+(vibr * 0.01), -1.f, 1.f, 0.f, 1.f));
    float car1 = vec1 -> play(venv1 , map(mod1+(vibr * 0.01), -1.f, 1.f, 0.f, 1.f));
    // float car = carrier.play(modulator.play()+(vib.play() * 0.01));
    float sig = car * env * amps[scoreptr & 7];
    float sig1 = car1 * env1 * amps1[scoreptr1 % 4];

    del = delay.play(sig + sig1, 0.4 + (vibr * 0.001), 0.4, 0.5);
    rev = verb.play(sig + sig1 + del, 0.9995);
    //
    float left = ((sig * 0.8) + (del * 0.54) + (rev * 0.74));
    float right = ((sig1 * 0.8) + (del * 0.54) + (rev * 0.74));

    // Stereo frame: two increments of out buffer
    *out++ = left; 
    *out++ = right;

    // *out++ = sig; 
    // *out++ = sig1;
    
#ifdef DEBUG
    printf("output: %f\n", sig);
#endif

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

  for (int i = 0; i < 12; i++){
    score[i] *= 2;
  }
  range(amps, 9, 0, 0.8, 0, 0.2);
  range(amps1, 4, 0, 0.8, 0, 0.2);
  range(dur, 21, 0, 0.8, 0, 2.2);
  range(dur1, 5, 0, 0.8, 0, 2.2);


  std::vector<Wavetable> t = {*w0, *w1, *w2, *w3};
  vec = new VectorOscillator(t, interpolation::cubic);
  vec1 = new VectorOscillator(t, interpolation::cubic);

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
