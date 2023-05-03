#ifndef PI
const float pi = 3.14159265358979323846;
#define PI
#endif

struct frame {
  float left;
  float right;
};

enum WAVESHAPE {
  SINE,
  TRIANGLE,
  SAW,
  SQUARE,
  ENV
};

class WaveTable {

  public:


	// Entry-point to let the WaveTable oscillator play. Does interpolation and movement of
    // readpointer under the hood.
	float play();
  
    void init(int sampleRate);
    void init(float * wavetable, int sampleRate);
    // Entry-point to let the WaveTable oscillator play. Does interpolation and movement of
    // readpointer under the hood.
    // ----
    //
    // Includes access to phase of the wavetable readpointer, allowing for 
    //
    // Frequency Modulation
//	float play(float phase);

    void setFreq(float);
    void setWave(WAVESHAPE wave);
    
    const float getSampleL(const float p) const;
    float getSampleL(float p);
    const float getSampleCos(const float p) const;

    // Performs an interpolation between samples to determine what value to return when the 
    // read-pointer tries to read the value two samples.
    // ----
    // -- LINEAR : Weighs two points relative to the readpointer. 
    // ----
    //
    // Uses the WaveTable member **interpolationType** 

    // Determines which sample to read next, taking into account the oscillators frequency and the
    // global samplerate, then updates the WaveTable-structs *position* member. 
    // ----
    //
    // This also allows for an optional *phase* input, easily translated to **frequency modulation** (
    // FM ).
    void movePointer(float phase);

    // Determines which sample to read next, taking into account the oscillators frequency and the
    // global samplerate, then updates the WaveTable-structs *position* member. 
    void movePointer();

    
  private:
    float position;
    WAVESHAPE waveshape;
    float tableLength;
    float samplerate;
    float frequency;
    float table[1024];
    
    // Creates one of several simple WaveTable shapes, chosen by the WAVESHAPE argument
    //
    //
    // -- SINE     : Sine waveform
    // -- TRIANGLE : Triangle waveform
    // -- SQUARE   : Square waveform
    // -- SAW      : Saw waveform
    // -- ENV      : Hanning window waveform ( squared sine ), used for Envelopes
    void populateTable(WAVESHAPE waveshape);
};
