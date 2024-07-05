#include <vector>
#include <string>
#include <iostream>
#include "../portaudio/include/portaudio.h"
#include "../dsp/dsp.h"
#include "../dsp/interpolation.hpp"
#include "../dsp/wavetable.hpp"
#include "../dsp/envelope.hpp"
#include "../dsp/verb.hpp"
#include "../dsp/delay.hpp"
#include "../dsp/waveshape.h"
#include "../dsp/vectortable.hpp"

// DURATION OF THE GENERATED TONE
const int DURATION =           120000; // milliseconds
// DEFAULT LENGHT OF THE WAVETABLE
const int SIZE =      512;
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
using namespace interpolation;


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

float* tables[4] = {table0, table1, table2, table3};

VectorOscillator vec0 = VectorOscillator::init(SAMPLE_RATE);
VectorOscillator vec1= VectorOscillator::init(SAMPLE_RATE);

float v0_freq = 0.f;
float v1_freq = 0.f;
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

// Wavetable carrier = Wavetable(TRIANGLE, SIZE, SAMPLE_RATE, interpolation::cubic);
// Wavetable* modulator 
Wavetable modulator0 = Wavetable::init(SAMPLE_RATE);
Wavetable modulator1 = Wavetable::init(SAMPLE_RATE);
Wavetable vib = Wavetable::init(SAMPLE_RATE);

float mod0_t[SIZE +1] = {0.f};
float mod1_t[SIZE +1] = {0.f};
float  vib_t[SIZE +1] = {0.f};

float m0_freq = 0.f;
float m1_freq = 0.f;
float v_freq = 3.2f;

// Delay delay = Delay(SAMPLE_RATE, 4.f, 1, interpolation::cubic);

SchroederVerb verbl = SchroederVerb(SAMPLE_RATE);
SchroederVerb verbr = SchroederVerb(SAMPLE_RATE);

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
  float revl=0.f, revr=0.f;
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
      v0_freq = score[scoreptr % 13];
      m0_freq = score[scoreptr % 5] * 3; 
    } else {
      env = ampenv.play(GATE::off, 2.f);
      venv = vecenv.play(GATE::off, 3.f);
    }

    if (timeline == seq1) {
      scoreptr++;
      seq1 += (unsigned)(SAMPLE_RATE * dur1[scoreptr1 % 5]);
      env1 = ampenv1.play(GATE::on);
      venv1 = vecenv1.play(GATE::on);
      v1_freq = score1[scoreptr1 % 4];
      m1_freq = score1[scoreptr1 % 4] * 3;
    } else {
      env1 = ampenv1.play(GATE::off, 2.f);
      venv1 = vecenv1.play(GATE::off, 3.f);
    }

    float vibr = vib.play<SIZE, linear>(vib_t, v_freq, 0.f);

    float mod0 = modulator0.play<SIZE, linear>(mod0_t, m0_freq, 0.f);
    float mod1 = modulator1.play<SIZE, linear>(mod1_t, m1_freq, 0.f);

    // Sound generation section
    float car = vec0.play<SIZE, 4, cubic>(
        tables,
        v0_freq,
        venv,
        map(mod0+(vibr * 0.01), -1.f, 1.f, 0.f, 1.f)
      );

    float car1 = vec1.play<SIZE, 4, cubic>(
        tables,
        v1_freq,
        venv,
        map(mod1+(vibr * 0.01), -1.f, 1.f, 0.f, 1.f)
      );
    // float car = carrier.play(modulator.play()+(vib.play() * 0.01));
    float sig = car * env * amps[scoreptr & 7];
    float sig1 = car1 * env1 * amps1[scoreptr1 % 4];

    // del = delay.process(sig + sig1, 0.5);
    // revl = verbl.process(sig + del, 0.8);
    // revr = verbr.process(sig1 - del, 0.8);
    revl = verbl.process(sig, 0.8);
    revr = verbr.process(sig1, 0.8);

    //
    // float left = ((sig * 0.8) + (del * 0.54) + (revl * 0.74));
    // float right = ((sig1 * 0.8) + (del * 0.54) + (revr * 0.74));
    
    float left = ((sig * 0.8) + (revl * 0.74));
    float right = ((sig1 * 0.8) + (revr * 0.74));

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

  complex_sine(tables[0], SIZE, amp0, phs0, 1);
  complex_sine(tables[1], SIZE, amp1, phs1, 3);
  complex_sine(tables[2], SIZE, amp2, phs2, 5);
  complex_sine(tables[3], SIZE, amp3, phs3, 7);

  triangle(mod0_t, SIZE);
  triangle(mod1_t, SIZE);
  sine(vib_t, SIZE);

  for (int i = 0; i < 12; i++){
    score[i] *= 2;
  }
  range(amps, 9, 0, 0.8, 0, 0.2);
  range(amps1, 4, 0, 0.8, 0, 0.2);
  range(dur, 21, 0, 0.8, 0, 2.2);
  range(dur1, 5, 0, 0.8, 0, 2.2);

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
