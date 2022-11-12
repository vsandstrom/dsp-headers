#include "portaudio.h"
#include <iostream>
#include <cmath>

#define DURATION		(4000) // milliseconds
#define SAMPLE_RATE		(48000)
#define TABLE_LEN       512
#define AMP             0.2f
#define FREQ            30.0f

#ifdef DEBUG
    #define D(x) x
#else
    #define D(x)
#endif
// TODO: 
// experiment with a wavetable struct that keeps track of its own position,

typedef struct {
	float left;
	float right;
} frame;

enum WAVETYPES {
    SINE = 0,
    TRIANGLE,
    SAW,
    SQUARE,
    ENV
};

// instead of #defined math macros
const double PI = 3.14159265358979323846;

double TABLE[512] = {0};
double ENV_TABLE[512] = {0};

// Initialize position as global value
// if you don't want to use static position variables, create them as global values. 
double pos = 0;
double samplePerHertz = 0, steps = 0;
int bPos = 0, fPos = 0;

// Fill Wavetable with values
void populateTable(double*, int, int);

// callback function must contain these inputs as PortAudio expects it.
static int paCallback(  const void* inputBuffer,						// input
						void* outputBuffer,								// output
						unsigned long framesPerBuffer,					// length of buffer in frames
						const PaStreamCallbackTimeInfo* timeinfo,		// :
						PaStreamCallbackFlags statusFlags,
						void* userdata )								// "void"-type can be typecast to other
{

	// cast data passing through stream
	frame* data = (frame*) userdata;
	float* out = (float*) outputBuffer;
	unsigned int i;
	(void) inputBuffer; // prevent unused variable warning
    
    
    // 512 samples är en hertz, hur många samples får det plats på en hertz när tonhöjden ska vara 440Hz 
    // i en samplerate av 48000. våglängden är då samplerate / hertz -> samples per våglängd

    double freq = FREQ;
    double diff = 0, bWeight = 0, fWeight = 0;
    
    // initialize first value, no wierd garbage value
    // if they are initialized here, make sure to give the variables the correct values
    // before using it, otherwise there will be a '0'-sample at the start of each block.
    data->left = 0;
    data->right = 0;

	// Write saw waveform to buffer
	for (i = 0; i < framesPerBuffer; i++) { // loop over buffer
        // iterpolate
        diff = floor(pos);
        bWeight = pos - diff;
        fWeight = 1 - bWeight;
        bPos = (int)(pos-1.f);
        fPos = (int)pos;
        if (bPos < 0) {
            bPos = 511;
        }

        data -> left = (TABLE[bPos] * fWeight) + (TABLE[fPos] * bWeight);
        data -> right = (TABLE[bPos] * fWeight) + (TABLE[fPos] * bWeight);

        *out++ = data -> left * AMP; // LEFT			-- travel through buffer and fill with samples
		*out++ = data -> right * AMP; // RIGHT		-- 'out' variable is dereferenced before filled

        // ta reda på hur många samples en våglängd i angiven frekvensen motsvarar, 
        double samplePerHertz = (double)SAMPLE_RATE / freq;
        double steps = (double)TABLE_LEN / samplePerHertz;
        pos += steps;

        if (pos > TABLE_LEN) {
            pos -= TABLE_LEN;
        }

	}
	return 0;
}



static frame data;


int main(void) {

    populateTable(TABLE, TABLE_LEN, SINE);
    populateTable(ENV_TABLE, TABLE_LEN, ENV);
    D(std::cout << "Populated tables");

	PaStream* stream;
	PaError err;

    D(std::cout << "PortAudio Test.\n";);

	// data here is not pointer, we access member variables directly, i. e '.' instead of '->'
	data.left = data.right = 0.0;

	err = Pa_Initialize();
	if ( err != paNoError ) goto error;
    

	// open an audio I/O stream:
	err = Pa_OpenDefaultStream( &stream,  // < --- Callback is in err
								0, 
								2,
								paFloat32,
								SAMPLE_RATE,
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


void populateTable(double* table, int n, int tabletype ) {
    float inc = 0, angle = 0;
    int i = 0;

    switch (tabletype) {
        case (SINE) : {
            inc = PI * 2  / (float) n;

            for (i = 0; i < n; ++i) {
                *table++ = sin(angle);
                angle += inc;
            }
            D(printf("SINE\n"));
            break;
        }

        case (SAW) : {
            inc = 2.0f / (float) n;
            for (i = 0; i < n; ++i) {
                *table++ =  angle - 1.0f;
                angle += inc;
            }
            D(printf("SAW\n"));
            break;
        }

        case (ENV) : {
            // A hanning window style envelope (squared sine)
            inc = PI / (float) n;
            for (i = 0; i < n; ++i) {
                *table++ = 1.0f - cos(angle) * cos(angle); 
                angle += inc;
            }
            D(printf("ENV: 1 - cos(x)^2\n"));
            break;
        }

    }
}

