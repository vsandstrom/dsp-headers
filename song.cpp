#include <portaudio.h>
#include <vector>
#include "score.h"
#include "dsp/dsp.h"
#include "dsp/interpolation.hpp"
#include "dsp/wavetable.hpp"
#include "dsp/envelope.hpp"
#include "dsp/filter.hpp"
#include "dsp/waveshape.h"
#include "dsp/vectoroscillator.hpp"
#include "dsp/delay.hpp"
#include "dsp/verb.hpp"

using namespace dspheaders;

// keeps track of the number of the current sample
unsigned timeline = 0;
// Progresses the score
unsigned scoreptr = 0;
unsigned seq = (unsigned)(DUR[scoreptr] * SAMPLE_RATE);

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
Envelope ampenv = Envelope(ap, 4, at, 3, SAMPLE_RATE, interpolation::cubic);

// Vector Movement Envelope
float vp[] = {0.f, 0.8f, 0.3f, 0.f};
float vt[] = {1.1f, 0.8f, 1.4f};
Envelope vecenv = Envelope(vp, 4, vt, 3, SAMPLE_RATE, interpolation::linear);

// Wavetable carrier = Wavetable(TRIANGLE, TABLE_LEN, SAMPLE_RATE, interpolation::cubic);
// Wavetable* modulator 
Wavetable modulator = Wavetable(TRIANGLE, 512, SAMPLE_RATE, interpolation::cubic);
Wavetable vibrato = Wavetable(SINE, 512, SAMPLE_RATE, interpolation::cubic);

Delay delay = Delay(SAMPLE_RATE, 4.f, 4, interpolation::cubic);
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
      seq += (unsigned)(SAMPLE_RATE * DUR[scoreptr % 18]);
      env = ampenv.play(GATE::on);
      venv = vecenv.play(GATE::on);
      vec -> frequency = SCORE[scoreptr % 5];
      modulator.frequency = SCORE[scoreptr % 3] * 7/2; 
    } else {

      env = ampenv.play(GATE::off, 2.f);
      venv = vecenv.play(GATE::off, 3.f);

    }

    float vibr = vibrato.play();
    float mod = modulator.play();

    float car = vec -> play(venv, map(mod + (vibr * 0.01), -1.f, 1.f, 0.f, 1.f));
    float sig = car * env * AMP[scoreptr & 7];

    // del = delay.play(sig, 0.8, 0.2, 0.5);
    rev = verb.play(sig + del, 0.9995);

    float left = (sig * 0.2) + (del * 0.14)+ rev * 0.04;
    float right = (sig * 0.2) + (del * 0.14) + rev * 0.04;

    // Stereo frame: two increments of out buffer
    *out++ = left; 
    *out++ = right;
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

  std::vector<Wavetable> vectortable = {*w0, *w1, *w2, *w3};
  vec = new VectorOscillator(vectortable, interpolation::cubic);
  vec->frequency = 0;
  modulator.frequency = 0;
  vibrato.frequency = 3.2f;
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
