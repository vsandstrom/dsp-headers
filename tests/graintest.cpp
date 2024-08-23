#include <memory>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "../portaudio/include/portaudio.h"
#include "../dsp/dsp.h"
#include "../dsp/envelope.hpp"
#include "../dsp/interpolation.hpp"
#include "../dsp/wavetable.hpp"
// #include "../dsp/grain.hpp"
#include "../dsp/grain2.hpp"
#include "../dsp/interpolation.hpp"
#include "../dsp/trigger.hpp"

#ifdef DEBUG
  #define D(x) x
#else
  #define D(x) 
#endif

// SETUP
const int INPUT_CH = 2;
const int OUTPUT_CH = 2;
const int MAX_GRAINS = 16;
const float GRAIN_DUR = 0.01f;
const float INTERVAL = 0.1f;
const float RATE = 1.f; // TRY NEGATIVE
const float JITTER = 0.6f;
const float PITCH_MOD_AMOUNT = 0.004f;
const float RECORD_LEN = 4.f; // seconds

const int PROGRAM_DURATION = 120000; // milliseconds
const float  SAMPLE_RATE =   48000;

const size_t BUFSIZE = 48000 * 4;
using namespace dspheaders;

// GLOBALS
Granulator2<16, 4*48000> * gr; // = Granulator2<MAX_GRAINS, BUFSIZE>();
Envelope* genv;
std::shared_ptr<Buffer> buf(new Buffer(RECORD_LEN, SAMPLE_RATE, interpolation::linear));
Impulse trigger = Impulse(INTERVAL, SAMPLE_RATE);
// Dust trigger = Dust(INTERVAL, SAMPLE_RATE);
Wavetable ph_saw = Wavetable(SAW, 1024, SAMPLE_RATE, interpolation::linear);
Wavetable lfo = Wavetable(SINE, 1024, SAMPLE_RATE, interpolation::linear);

// Values setting the Grains size in seconds
float p[4] = { 0.01f, 0.8f, 2.2f, 0.3f };
// Time between the values
float t[3] = { 12.f, 12.f, 18.f };
// curve angle between values
float c[3] = { 0.8f, 0.2f, 1.2f };

Envelope size = Envelope(p, 4, t, 3, c, 3, SAMPLE_RATE, interpolation::linear);

// Values setting the Grains speed of the playhead
float p2[4] = { 1.f, 0.8f, 2.2f, 0.3f };
// Values determining the transport between values
float t2[3] = { 6.f, 20.f, 18.f };
float c2[3] = { 0.8f, 0.2f, 1.2f };

Envelope speed = Envelope(p2, 4, t2, 3, c2, 3, SAMPLE_RATE, interpolation::linear);

// Values setting the Grains speed of the playhead
float p3[4] = { 0.1f, 0.03f, 2.2f, 0.3f };
float t3[3] = { 12.f, 8.f, 18.f };
float c3[3] = { 1.7f, 1.2f, 3.f };

Envelope imp = Envelope(p3, 4, t3, 3, c3, 3, SAMPLE_RATE, interpolation::linear);

bool toggle_size_env = true;
bool toggle_rate_env = true;
bool toggle_impl_env = true;

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
  float r = 0.f;
  float t = 0.f;


	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
    if (writeptr < buf->bufferlength) {
      float input = *in++;
      buf->writesample(input, writeptr % buf->bufferlength);
      writeptr++;
      in++; // skip one since input buffer is stereo
      *out++ = 0.f;
      *out++ = 0.f;
    } else {
      if (toggle_size_env || size.finished()) {
        s = size.play(GATE::on);
        toggle_size_env = false;
      } else {
        s = size.play(GATE::off);
      }

      if (toggle_rate_env || speed.finished()) {
        r = speed.play(GATE::on);
        toggle_rate_env = false;
      } else {
        r = speed.play(GATE::off);
      }
      
      if (toggle_impl_env || imp.finished()) {
        t = imp.play(GATE::on);
        toggle_impl_env = false;
      } else {
        t = imp.play(GATE::off);
      }
      
        float trig = trigger.play(t);
        float phasor = map(ph_saw.play(),-1.f, 1.f, 0.f, 0.99f);
        // gr->setGrainSize(s);
        gryn = gr->play(
          phasor, // TRY STATIC VALUE (0.0 <= x < 1.0)
          0.2,
          RATE + (lfo.play() * PITCH_MOD_AMOUNT),
          r + (lfo.play() * PITCH_MOD_AMOUNT),
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
          "    │ FOR A %*.*f OF SECONDS, THEN PLAYS THE AUDIO BACK     │\n" 
          "    │ THROUGH A GRANULATED AUDIO EFFECT.                  │\n"
          "    │                                                     │\n"
          "    │           ( NOTHING IS SAVED TO MEMORY )            │\n"
          "    ╘═════════════════════════════════════════════════════╛\n\n", 1, 1, RECORD_LEN);

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
  
  const size_t vall = 3;
  const size_t durl = 2;

  float val[vall] = {0.0, 1.0, 0.0};
  float dur[durl] = {0.2, 0.2};
  float cur[durl] = {1.0, 1.0};

  struct BreakPoints<vall, durl> brk = BreakPoints<vall, durl>(
    val,
    dur,
    cur
  );

  gr = new Granulator2<16, 4*48000>(
    brk,
    SAMPLE_RATE,
    interpolation::linear
  );

  // gr->setJitter(JITTER);
  // gr->setNumGrains(MAX_GRAINS);
  
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
